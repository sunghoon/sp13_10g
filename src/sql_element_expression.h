#pragma once
//  Copyright [2013] Kyle Galloway (kyle.s.galloway@gmail.com)
//                   Pravish Sood (pravish.sood@gmail.com)
//                   Dylan Kelemen (dckelemen@gmail.com)

//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at

//      http://www.apache.org/licenses/LICENSE-2.0

//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
#ifndef MONGOODBC_SQL_ELEMENT_EXPRESSION_H_
#define MONGOODBC_SQL_ELEMENT_EXPRESSION_H_

#include "sql_element_column_name.h"

#include <boost/fusion/adapted.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/variant/variant.hpp>

#include <ostream>
#include <string>
#include <vector>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace spirit = boost::spirit;
namespace phoenix = boost::phoenix;

namespace mongoodbc {

class SQLElementExpression;

template <typename It>
class SQLElementExpressionParser;

typedef boost::variant<
    SQLElementColumnName,
    char,
    std::string> SQLElementExpression_Primary;

template <typename It>
struct SQLElementExpression_PrimaryParser
    : qi::grammar<It, SQLElementExpression_Primary()> {

    qi::rule<It, std::string()> _quotedString;
    qi::rule<It, SQLElementExpression_Primary()> _rule;
    SQLElementExpressionParser<It> *_exprParser;
    SQLElementColumnNameParser<It> _columnNameParser;

    SQLElementExpression_PrimaryParser(SQLElementExpressionParser<It> *);
};

template <typename It>
SQLElementExpression_PrimaryParser<It>::SQLElementExpression_PrimaryParser(
    SQLElementExpressionParser<It> *exprParser)
    : SQLElementExpression_PrimaryParser::base_type(_rule)
    , _exprParser(exprParser)
{
    _quotedString = ascii::char_('"') 
                    >> *(ascii::alnum |
                         ascii::space |
                         "\"\"" |
                         ascii::char_(";:'?/\\|,.<>!@#$%^&*()-_+=[]{}~`"))
                    >> ascii::char_('"');

    _rule = _columnNameParser._rule|
             ascii::char_('?') |
             _quotedString;

    BOOST_SPIRIT_DEBUG_NODE(_rule);
};


/*
* In memory representation of a factor.
*/
struct SQLElementExpression_Factor {
    char _op;
    SQLElementExpression_Primary _primary;
};

template <typename It>
struct SQLElementExpression_FactorParser
    : qi::grammar<It, SQLElementExpression_Factor()> {

    qi::rule<It, SQLElementExpression_Factor()> _rule;
    SQLElementExpression_PrimaryParser<It> _primaryParser;

    SQLElementExpression_FactorParser(SQLElementExpressionParser<It> *);
};

template <typename It>
SQLElementExpression_FactorParser<It>::SQLElementExpression_FactorParser(
    SQLElementExpressionParser<It> *exprParser)
    : SQLElementExpression_FactorParser::base_type(_rule)
    , _primaryParser(exprParser)
{
    _rule = -ascii::char_("+-") [phoenix::at_c<0>(qi::_val) = qi::_1]
            >> _primaryParser._rule [phoenix::at_c<1>(qi::_val) = qi::_1];

    BOOST_SPIRIT_DEBUG_NODE(_rule);
};

/*
* In memory representation of a term.
*/
struct SQLElementExpression_Term {
    std::vector<boost::recursive_wrapper<SQLElementExpression_Term> > _term;
    char _op;
    SQLElementExpression_Factor _factor;
};

template <typename It>
struct SQLElementExpression_TermParser
    : qi::grammar<It, SQLElementExpression_Term(), ascii::space_type> {

    qi::rule<It, SQLElementExpression_Term(), ascii::space_type> _rule;
    SQLElementExpression_FactorParser<It> _factorParser;

    SQLElementExpression_TermParser(SQLElementExpressionParser<It> *);
};

template <typename It>
SQLElementExpression_TermParser<It>::SQLElementExpression_TermParser(
    SQLElementExpressionParser<It> *exprParser)
    : SQLElementExpression_TermParser::base_type(_rule)
    , _factorParser(exprParser)
{
    _rule = _factorParser._rule [phoenix::at_c<2>(qi::_val) = qi::_1] |
            _rule [phoenix::push_back(phoenix::at_c<0>(qi::_val), qi::_1)] 
                >> ascii::char_("+-") [phoenix::at_c<1>(qi::_val) = qi::_1]
                >> _factorParser._rule [phoenix::at_c<2>(qi::_val) = qi::_1];

    BOOST_SPIRIT_DEBUG_NODE(_rule);
};

/*
* In memory representation of an SQL expression.
*/
struct SQLElementExpression {
    std::vector<boost::recursive_wrapper<SQLElementExpression> > _expr;
    char _op;
    SQLElementExpression_Term _term;
};
inline std::ostream& operator<<(std::ostream& stream, const SQLElementExpression& rhs);

template <typename It>
struct SQLElementExpressionParser : qi::grammar<It, SQLElementExpression(), ascii::space_type> {
    qi::rule<It, SQLElementExpression(), ascii::space_type> _rule;
    SQLElementExpression_TermParser<It> _termParser;

    SQLElementExpressionParser();
};

template <typename It>
SQLElementExpressionParser<It>::SQLElementExpressionParser()
    : SQLElementExpressionParser::base_type(_rule)
    , _termParser(this)
{
    _rule = _termParser._rule [phoenix::at_c<2>(qi::_val) = qi::_1] |
            _rule [phoenix::push_back(phoenix::at_c<0>(qi::_val), qi::_1)] 
                >> ascii::char_("/*") [phoenix::at_c<1>(qi::_val) = qi::_1]
                >> _termParser._rule [phoenix::at_c<2>(qi::_val) = qi::_1];

    BOOST_SPIRIT_DEBUG_NODE(_rule);
};

} // close mongoodbc namespace

BOOST_FUSION_ADAPT_STRUCT(mongoodbc::SQLElementExpression_Factor,
                          (char, _op)
                          (mongoodbc::SQLElementExpression_Primary, _primary));

BOOST_FUSION_ADAPT_STRUCT(mongoodbc::SQLElementExpression_Term,
                          (std::vector<boost::recursive_wrapper<mongoodbc::SQLElementExpression_Term> >, _term)
                          (char, _op)
                          (mongoodbc::SQLElementExpression_Factor, _factor));

BOOST_FUSION_ADAPT_STRUCT(mongoodbc::SQLElementExpression,
                          (std::vector<boost::recursive_wrapper<mongoodbc::SQLElementExpression> >, _expr)
                          (char, _op)
                          (mongoodbc::SQLElementExpression_Term, _term));

inline std::ostream& mongoodbc::operator<<(std::ostream& stream,
                                           const mongoodbc::SQLElementExpression& rhs)
{
    return stream;
}

#endif

