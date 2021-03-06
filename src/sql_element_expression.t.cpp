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

#include "sql_element_expression.h"

#include <gtest/gtest.h>

#include <iostream>

TEST(ElementExpression_PrimaryParseTest, DynamicParameter)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_PrimaryParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Primary primary;
    std::string str("?");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, primary));
        EXPECT_FALSE(primary._columnName);
        EXPECT_TRUE(primary._dynamicParameter);
        EXPECT_FALSE(primary._literal);
        EXPECT_EQ(0, primary._expr.size());
        EXPECT_EQ('?', *primary._dynamicParameter);
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_PrimaryParseTest, ColumnName)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_PrimaryParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Primary primary;
    std::string str("column");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, primary));
        EXPECT_TRUE(primary._columnName);
        EXPECT_FALSE(primary._dynamicParameter);
        EXPECT_FALSE(primary._literal);
        EXPECT_EQ(0, primary._expr.size());
        EXPECT_EQ("column", (*primary._columnName)._columnName);
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_PrimaryParseTest, TableNameColumnName)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_PrimaryParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Primary primary;
    std::string str("table.column");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, primary));
        EXPECT_TRUE(primary._columnName);
        EXPECT_FALSE(primary._dynamicParameter);
        EXPECT_FALSE(primary._literal);
        EXPECT_EQ(0, primary._expr.size());
        EXPECT_EQ("column", (*primary._columnName)._columnName);
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_PrimaryParseTest, StringLiteral)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_PrimaryParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Primary primary;
    std::string str("\"literal\"");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, primary));
        EXPECT_FALSE(primary._columnName);
        EXPECT_FALSE(primary._dynamicParameter);
        EXPECT_TRUE(primary._literal);
        EXPECT_EQ(0, primary._expr.size());
        EXPECT_EQ("literal", *primary._literal);
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_PrimaryParseTest, NumericLiteral)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_PrimaryParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Primary primary;
    std::string str("123");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, primary));
        EXPECT_FALSE(primary._columnName);
        EXPECT_FALSE(primary._dynamicParameter);
        EXPECT_FALSE(primary._literal);
        EXPECT_TRUE(primary._num);
        EXPECT_EQ(0, primary._expr.size());
        EXPECT_EQ(123, *primary._num);
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_PrimaryParseTest, StringLiteralDoubleQuote)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_PrimaryParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Primary primary;
    std::string str("\"lite\"\"ral\"\"\"");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, primary));
        EXPECT_FALSE(primary._columnName);
        EXPECT_FALSE(primary._dynamicParameter);
        EXPECT_TRUE(primary._literal);
        EXPECT_EQ(0, primary._expr.size());
        EXPECT_EQ("lite\"ral", *primary._literal);
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_FactorParseTest, StringLiteral)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_FactorParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Factor factor;
    std::string str("\"literal\"");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, factor));
        mongoodbc::SQLElementExpression_Primary& primary = factor._primary;
        EXPECT_FALSE(primary._columnName);
        EXPECT_FALSE(primary._dynamicParameter);
        EXPECT_TRUE(primary._literal);
        EXPECT_EQ(0, primary._expr.size());
        EXPECT_EQ("literal", *primary._literal);

    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_FactorParseTest, ColumnName)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_FactorParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Factor factor;
    std::string str("column");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, factor));
        mongoodbc::SQLElementExpression_Primary& primary = factor._primary;
        EXPECT_TRUE(primary._columnName);
        EXPECT_FALSE(primary._dynamicParameter);
        EXPECT_FALSE(primary._literal);
        EXPECT_EQ(0, primary._expr.size());
        EXPECT_EQ("column", (*primary._columnName)._columnName);
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_FactorParseTest, TableNameColumnName)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_FactorParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Factor factor;
    std::string str("table.column");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, factor));
        mongoodbc::SQLElementExpression_Primary& primary = factor._primary;
        EXPECT_TRUE(primary._columnName);
        EXPECT_FALSE(primary._dynamicParameter);
        EXPECT_FALSE(primary._literal);
        EXPECT_EQ(0, primary._expr.size());
        EXPECT_EQ("column", (*primary._columnName)._columnName);
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_TermParseTest, StringLiteral)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_TermParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Term term;
    std::string str("\"literal\"");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, term));
        mongoodbc::SQLElementExpression_Factor& factor = term._factor;
        mongoodbc::SQLElementExpression_Primary& primary = factor._primary;
        EXPECT_FALSE(primary._columnName);
        EXPECT_FALSE(primary._dynamicParameter);
        EXPECT_TRUE(primary._literal);
        EXPECT_FALSE(primary._num);
        EXPECT_EQ(0, primary._expr.size());
        EXPECT_EQ("literal", *primary._literal);
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_TermParseTest, NumericLiteral)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_TermParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Term term;
    std::string str("123");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, term));
        mongoodbc::SQLElementExpression_Factor& factor = term._factor;
        mongoodbc::SQLElementExpression_Primary& primary = factor._primary;
        EXPECT_FALSE(primary._columnName);
        EXPECT_FALSE(primary._dynamicParameter);
        EXPECT_FALSE(primary._literal);
        EXPECT_TRUE(primary._num);
        EXPECT_EQ(0, primary._expr.size());
        EXPECT_EQ(123, *primary._num);
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_TermParseTest, NegativeNumericLiteral)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_TermParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Term term;
    std::string str("-123");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, term));
        mongoodbc::SQLElementExpression_Factor& factor = term._factor;
        mongoodbc::SQLElementExpression_Primary& primary = factor._primary;
        EXPECT_FALSE(primary._columnName);
        EXPECT_FALSE(primary._dynamicParameter);
        EXPECT_FALSE(primary._literal);
        EXPECT_TRUE(primary._num);
        EXPECT_EQ(0, primary._expr.size());
        EXPECT_EQ(123, *primary._num);
        EXPECT_EQ('-', factor._op);
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_TermParseTest, ColumnName)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_TermParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Term term;
    std::string str("column");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, term));
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpression_TermParseTest, TableNameColumnName)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> exprParser;
    mongoodbc::SQLElementExpression_TermParser<std::string::const_iterator> parser(&exprParser);
    mongoodbc::SQLElementExpression_Term term;
    std::string str("table.column");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, term));
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpressionParseTest, StringLiteral)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> parser;
    mongoodbc::SQLElementExpression expr;
    std::string str("\"literal\"");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, expr));
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpressionParseTest, ColumnName)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> parser;
    mongoodbc::SQLElementExpression expr;
    std::string str("column");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, expr));
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

TEST(ElementExpressionParseTest, TableNameColumnName)
{
    mongoodbc::SQLElementExpressionParser<std::string::const_iterator> parser;
    mongoodbc::SQLElementExpression expr;
    std::string str("table.column");
    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    try
    {
        EXPECT_TRUE(
            boost::spirit::qi::phrase_parse(iter, end, parser, boost::spirit::ascii::space, expr));
    }
    catch (const boost::spirit::qi::expectation_failure<std::string::const_iterator>& ex)
    {
        std::string fragment(ex.first, ex.last);
        std::cerr << ex.what() << "'" << fragment << "'" << std::endl;
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

