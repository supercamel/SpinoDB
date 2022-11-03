//  Copyright 2022 Sam Cowen <samuel.cowen@camelsoftware.com>
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.

#include "query_parser.h"

#include <sstream>
#include <set>
#include "collection.h"

using namespace std;

using namespace Spino;

QueryParser::QueryParser(const std::vector<Index> &indicies, const char *query) : indicies(indicies)
{
    query_string = query;
    cursor = 0;
    instructions.reserve(10);
}


Token QueryParser::peek()
{
    uint32_t tcursor = cursor;
    Token ret = lex();
    cursor = tcursor;
    return ret;
}

/**
 * The lexer progressively reads the input text and turns it into Tokens.
 * Each call to lex() will return a token and move along the input string.
 * When there is nothing left to read, lex() will return a token with value TOK_EOF
 */
inline Token QueryParser::lex()
{
    // while there are characters left to consume
    while (curc() != '\0')
    {
        // get the current character
        switch (curc())
        {
        // chew up white space
        case '\r':
        case '\n':
        case ' ':
        case '\t':
            next();
            continue;
        case '{':
            next();
            return Token(TOK_LH_BRACE, nullptr, 0);
        case '}':
            next();
            return Token(TOK_RH_BRACE, nullptr, 0);
        case '[':
            next();
            return Token(TOK_LH_BRACKET, nullptr, 0);
        case ']':
            next();
            return Token(TOK_RH_BRACKET, nullptr, 0);
        case ':':
            next();
            return Token(TOK_COLON, nullptr, 0);
        case '.':
            next();
            return Token(TOK_PERIOD, nullptr, 0);
        case ',':
            next();
            return Token(TOK_COMMA, nullptr, 0);
        case '"':
        {
            const char *start = &cursor_ptr()[1];
            size_t len = read_string_literal();
            return Token(TOK_STRING_LITERAL, start, len);
        }
        break;
        case '$':
        {
            next();
            const char *start = cursor_ptr();
            read_identifier();
            if (strncmp(start, "eq", 2) == 0)
            {
                return Token(TOK_EQUAL, nullptr, 0);
            }
            else if (strncmp(start, "gt", 2) == 0)
            {
                return Token(TOK_GREATER_THAN, nullptr, 0);
            }
            else if (strncmp(start, "lt", 2) == 0)
            {
                return Token(TOK_LESS_THAN, nullptr, 0);
            }
            else if (strncmp(start, "and", 3) == 0)
            {
                return Token(TOK_AND, nullptr, 0);
            }
            else if (strncmp(start, "or", 2) == 0)
            {
                return Token(TOK_OR, nullptr, 0);
            }
            else if (strncmp(start, "ne", 2) == 0)
            {
                return Token(TOK_NE, nullptr, 0);
            }
            else if (strncmp(start, "nin", 3) == 0)
            {
                return Token(TOK_NIN, nullptr, 0);
            }
            else if (strncmp(start, "not", 3) == 0)
            {
                return Token(TOK_NOT, nullptr, 0);
            }
            else if (strncmp(start, "in", 2) == 0)
            {
                return Token(TOK_IN, nullptr, 0);
            }
            else if (strncmp(start, "exists", 6) == 0)
            {
                return Token(TOK_EXISTS, nullptr, 0);
            }
            else if (strncmp(start, "type", 4) == 0)
            {
                return Token(TOK_TYPE, nullptr, 0);
            }
            else if (strncmp(start, "startsWith", 10) == 0)
            {
                return Token(TOK_STARTS_WITH, nullptr, 0);
            }
            else if (strncmp(start, "regex", 5) == 0)
            {
                return Token(TOK_REGEX, nullptr, 0);
            }
            else if (strncmp(start, "range", 5) == 0)
            {
                return Token(TOK_RANGE, nullptr, 0);
            }
            else
            {
                throw parse_error("Unknown $ operator " + string(start, 10));
            }
        }
        break;
        }
        // if the current character is alphabetic (a-z, A-Z)
        if (isalpha(curc()) || curc() == '_')
        {
            const char *start = cursor_ptr();
            // read an identifier
            size_t ident = read_identifier();
            // if its true/false then it's a boolean literal
            if ((strncmp(start, "true", 4) == 0) || (strncmp(start, "false", 5) == 0))
            {
                return Token(TOK_BOOL_LITERAL, start, 4);
            }
            // otherwise it's a field name

            return Token(TOK_FIELD_NAME, start, ident);
        }
        // if it's a number
        else if (isdigit(curc()) || (curc() == '-'))
        {
            // read a numeric literal
            const char *start = cursor_ptr();
            double value = read_numeric_literal();
            Token tok = Token(TOK_NUMERIC_LITERAL, cursor_ptr(), cursor_ptr() - start);
            tok.value = value;
            return tok;
        }
        // else throw an error because dunno what this is supposed to be
        throw parse_error("Expected character " + curc());
    }
    return Token(TOK_EOF, nullptr, 0);
}

// read identifier is usually for reading a field name
// like 'person.name'
size_t QueryParser::read_identifier()
{
    size_t counter = 0;
    while (curc() != '\0')
    {
        next();
        counter++;
        if ((!isalpha(curc())) && (!isdigit(curc())) && (curc() != '_') && (curc() != '.'))
        {
            return counter;
        }
    }
    throw parse_error("Unexpected end of query");
    return counter;
}

// read_string_literal() reads a string . .
// such a "Hello world"
size_t QueryParser::read_string_literal()
{
    size_t count = 0;
    next();
    while (curc() != '\0')
    {
        if (curc() == '"')
        {
            next();
            return count;
        }
        next();

        count++;
    }
    throw parse_error("Unexpected end of query");
    return count;
}

// read_numeric_literal
// returns a number in string format
// e.g. "12.5"
double QueryParser::read_numeric_literal()
{
    int len = 0;
    char *end;
    double result = strtod(cursor_ptr(), &end);
    len = end - cursor_ptr();
    cursor += len;
    return result;
}

DomNode* QueryParser::token_literal_to_node(Token t)
{
    switch (t.token)
    {
    case TOK_BOOL_LITERAL:
    {
        DomNode* node = dom_node_allocator.make();
        if (t.raw[0] == 't')
        {
            node->set_bool(true);
        }
        else
        {
            node->set_bool(false);
        }
        return node;
    }
    break;
    case TOK_STRING_LITERAL:
    {
        DomNode* node = dom_node_allocator.make();
        node->set_string(t.raw, t.len, true);
        return node;
    }
    break;
    case TOK_NUMERIC_LITERAL:
    {
        DomNode* node = dom_node_allocator.make();
        node->set_double(t.value);
        return node;
    }
    break;
    default:
    {
        return nullptr;
    }
    }
}

std::vector<Token>* QueryParser::parse_query(IndexIteratorRange &_range)
{
    instructions.clear();

    index_resolved = false;
    parse_expression();
    if (index_resolved)
    {
        _range = range;
    }
    else
    {
        _range.first = indicies[0].index.begin();
        _range.second = indicies[0].index.end();
    }
    return &instructions;
}

/* an expression can have the form
 * { <field_name> OR <string_litera>: <operator_expression> }
 * { $and/$or: { [ <expression1>, <expression2>,...<expressionN> ] }
 * { $not: { <expression> }}
 */
void QueryParser::parse_expression()
{
    if (lex().token == TOK_LH_BRACE)
    {
        auto tok = lex();
        if(tok.token == TOK_RH_BRACE) {
            return;
        }
        // if the token is a field, parse rhs
        else if ((tok.token == TOK_FIELD_NAME) || (tok.token == TOK_STRING_LITERAL))
        {
            Token field_name = tok;
            field_name.token = TOK_FIELD_NAME;
            tok = lex();
            if (tok.token != TOK_COLON)
            {
                throw parse_error("Expected a colon after identifier");
            }

            tok = peek();
            if ((tok.token == TOK_STRING_LITERAL) || (tok.token == TOK_NUMERIC_LITERAL))
            {
                parse_literal();
                if (index_resolved == false)
                {

                    for (auto &idx : indicies)
                    {
                        std::string s(field_name.raw, field_name.len);
                        if (s == idx.field_name)
                        {
                            Token t = instructions.back();
                            DomNode* top = token_literal_to_node(t);
                            instructions.pop_back();
                            range = idx.index.equal_range(*top);
                            dom_node_allocator.delete_object(top);
                            index_resolved = true;
                            if (lex().token != TOK_RH_BRACE)
                            {
                                throw parse_error("Missing close brace after field comparison");
                            }

                            return;
                        }
                    }

                    instructions.push_back(field_name);
                    instructions.push_back(Token(TOK_EQUAL));
                }
                else
                {
                    instructions.push_back(field_name);
                    instructions.push_back(Token(TOK_EQUAL));
                }

                if (lex().token != TOK_RH_BRACE)
                {
                    throw parse_error("Missing closing brace after basic field comparison");
                }
            }
            else
            {
                instructions.push_back(field_name);
                parse_operator_expression();
                tok = lex();
                if (tok.token != TOK_RH_BRACE)
                {
                    throw parse_error("Missing closing brace 5");
                }
            }
        }
        else if ((tok.token == TOK_AND) || (tok.token == TOK_OR))
        {
            Token op = tok;

            tok = lex();
            if (tok.token != TOK_COLON)
            {
                throw parse_error("Missing colon after and/or");
            }

            tok = lex();
            if (tok.token != TOK_LH_BRACKET)
            {
                throw parse_error("Expected lh bracket");
            }

            size_t count = 0;

            do
            {
                parse_expression();
                count++;

                tok = lex();
                if ((tok.token != TOK_RH_BRACKET) && (tok.token != TOK_COMMA))
                {
                    throw parse_error("Unexpected token in list.");
                }
            } while (tok.token != TOK_RH_BRACKET);

            Token growth(TOK_NUMERIC_LITERAL);
            growth.value = count;
            instructions.push_back(growth);

            if (lex().token != TOK_RH_BRACE)
            {
                throw parse_error("Missing closing brace 4");
            }

            instructions.push_back(op);
        }
        else if (tok.token == TOK_NOT)
        {
            if (lex().token != TOK_COLON)
            {
                throw parse_error("Expected colon after $not");
            }

            // since index lookup requires the documents to with within min and max range, 
            // if a sub-expression has an index field, then it will only search within this range
            // a not queries should also, probably, return results outside that range
            // so, we cannot use an index for a $not expression. not in the currently, anyway
            bool ir = index_resolved;
            index_resolved = true;

            parse_expression();
            index_resolved = ir;

            if (lex().token != TOK_RH_BRACE)
            {
                throw parse_error("Missing closing brace 3");
            }
            instructions.push_back(Token(TOK_NOT));
        }
        else {
            throw parse_error("Unexpected token");
        }
    }
    else
    {
        throw parse_error("Missing opening brace");
    }
}

/**
 * An operator expression can have the form
 * <literal> - this is the same as { $eq: <literal> }
 * { $eq/$ne/$gt/$lt: <literal> }
 * { $range: [<lower>, <upper>] }
 * { $in/$nin: <literal_list> }
 * { $exists: true/false }
 * { $type: number/string/bool/array/object }
 */
void QueryParser::parse_operator_expression()
{
    auto tok = peek();
    if (tok.token == TOK_LH_BRACE)
    {
        tok = lex();
        tok = lex();
        Token operation_tok = tok;
        if ((tok.token == TOK_EQUAL) ||
            (tok.token == TOK_NE) ||
            (tok.token == TOK_STARTS_WITH))
        {
            tok = lex();
            if (tok.token != TOK_COLON)
            {
                throw parse_error("Expected :");
            }

            parse_literal();

            instructions.push_back(operation_tok);

            tok = lex();
            if (tok.token != TOK_RH_BRACE)
            {
                throw parse_error("Missing closing brace 2");
            }
        }
        else if(tok.token == TOK_GREATER_THAN)
        {
            tok = lex();
            if(tok.token != TOK_COLON)
            {
                throw parse_error("Expected :");
            }

            parse_literal();

            tok = lex();
            if (tok.token != TOK_RH_BRACE)
            {
                throw parse_error("Missing closing brace 2");
            }
            if(index_resolved == false)
            {
                Token field = instructions[instructions.size()-2];
                std::string field_name(field.raw, field.len);
                for(auto& idx : indicies)
                {
                    if(field_name == idx.field_name)
                    {
                        DomNode* min = token_literal_to_node(instructions.back());
                        instructions.pop_back(); // pop the literal value
                        instructions.pop_back(); // pop the field name
                        range.first = idx.index.upper_bound(*min);
                        range.second = idx.index.end();
                        dom_node_allocator.delete_object(min);
                        index_resolved = true;
                        return;
                    }
                }
            }
            instructions.push_back(operation_tok);
        }
        else if(tok.token == TOK_LESS_THAN)
        {
            tok = lex();
            if(tok.token != TOK_COLON)
            {
                throw parse_error("Expected :");
            }

            parse_literal();

            tok = lex();
            if (tok.token != TOK_RH_BRACE)
            {
                throw parse_error("Missing closing brace 2");
            }
            if(index_resolved == false)
            {
                Token field = instructions[instructions.size()-2];
                std::string field_name(field.raw, field.len);
                for(auto& idx : indicies)
                {
                    if(field_name == idx.field_name)
                    {
                        DomNode* max = token_literal_to_node(instructions.back());
                        instructions.pop_back(); // pop the literal value
                        instructions.pop_back(); // pop the field name
                        range.first = idx.index.begin();
                        range.second = idx.index.lower_bound(*max);
                        dom_node_allocator.delete_object(max);
                        index_resolved = true;
                        return;
                    }
                }
            }
            instructions.push_back(operation_tok);
        }
        else if (tok.token == TOK_RANGE)
        {
            tok = lex();
            if (tok.token != TOK_COLON)
            {
                throw parse_error("Expected : after $range");
            }

            tok = lex();
            if (tok.token != TOK_LH_BRACKET)
            {
                throw parse_error("Expected LH bracket after $range:");
            }

            tok = peek();
            if (tok.token != TOK_NUMERIC_LITERAL)
            {
                throw parse_error("Expected numeric literal as lower bound of $range expression");
            }

            parse_literal();

            tok = lex();
            if (tok.token != TOK_COMMA)
            {
                throw parse_error("Expected comma after lower bound");
            }

            tok = peek();
            if (tok.token != TOK_NUMERIC_LITERAL)
            {
                throw parse_error("Expected numeric literal as upper bound of $range expression");
            }

            parse_literal();

            tok = lex();
            if (tok.token != TOK_RH_BRACKET)
            {
                throw parse_error("Expected rh bracket to close $range list");
            }

            tok = lex();
            if (tok.token != TOK_RH_BRACE)
            {
                throw parse_error("Missing closing brace");
            }

            if (index_resolved == false)
            {
                Token field = instructions[instructions.size()-3];
                std::string field_name(field.raw, field.len);
                for(auto& idx : indicies)
                {
                    if(field_name == idx.field_name)
                    {
                        DomNode* max = token_literal_to_node(instructions.back());
                        instructions.pop_back();
                        DomNode* min = token_literal_to_node(instructions.back());
                        instructions.pop_back();

                        range.first = idx.index.lower_bound(*min);
                        range.second = idx.index.upper_bound(*max);

                        dom_node_allocator.delete_object(min);
                        dom_node_allocator.delete_object(max);

                        instructions.pop_back(); //pop the field name
                        index_resolved = true;
                        return;
                    }
                }
            }
            instructions.push_back(Token(TOK_RANGE));
        }
        else if (tok.token == TOK_REGEX)
        {
            tok = lex();
            if (tok.token != TOK_COLON)
            {
                throw parse_error("Expected : after $regex");
            }

            tok = lex();
            if (tok.token != TOK_STRING_LITERAL)
            {
                throw parse_error("Expected string as regex parameter");
            }

            try
            {
                std::string raw(tok.raw, tok.len);
                if (raw.rfind("(?i)", 0) == 0)
                {
                    tok.regex = make_shared<std::regex>(raw.substr(4, std::string::npos), std::regex_constants::icase);
                }
                else
                {
                    tok.regex = make_shared<std::regex>(raw);
                }
            }
            catch (std::regex_error &err)
            {
                std::string errmsg = "Invalid regex: ";
                errmsg += err.what();
                throw parse_error(errmsg);
            }
            tok.token = TOK_REGEX;
            instructions.push_back(tok);

            tok = lex();
            if (tok.token != TOK_RH_BRACE)
            {
                throw parse_error("Missing closing brace 2");
            }

            instructions.push_back(Token(TOK_REGEX));
        }
        else if ((tok.token == TOK_IN) || (tok.token == TOK_NIN))
        {
            Token op = tok;

            tok = lex();
            if (tok.token != TOK_COLON)
            {
                throw parse_error("Expected colon after $in");
            }

            size_t start = instructions.size();
            parse_literal_list();
            size_t n = instructions.size() - start;

            Token list_length(TOK_NUMERIC_LITERAL);
            list_length.value = n;
            instructions.push_back(list_length);
            tok = lex();
            if (tok.token != TOK_RH_BRACE)
            {
                throw parse_error("Missing closing brace");
            }
            instructions.push_back(op);
        }
        else if (tok.token == TOK_EXISTS)
        {
            tok = lex();
            if (tok.token != TOK_COLON)
            {
                throw parse_error("Expected colon after $exists");
            }

            tok = lex();
            if (tok.token != TOK_BOOL_LITERAL)
            {
                throw parse_error("Expected true/false literal after $exists");
            }

            instructions.push_back(tok);

            tok = lex();
            if (tok.token != TOK_RH_BRACE)
            {
                throw parse_error("Missing closing brace");
            }
            instructions.push_back(Token(TOK_EXISTS));
        }
        else if (tok.token == TOK_TYPE)
        {
            tok = lex();
            if (tok.token != TOK_COLON)
            {
                throw parse_error("Expected colon after $type");
            }

            tok = lex();
            if (tok.token != TOK_FIELD_NAME)
            {
                throw parse_error("Expected type name after $type");
            }

            std::set<std::string> type_names = {
                "number",
                "string",
                "bool",
                "array",
                "object"};

            if (type_names.find(std::string(tok.raw, tok.len)) != type_names.end())
            {
                tok.token = TOK_STRING_LITERAL;
                instructions.push_back(tok);
            }
            else
            {
                throw parse_error("Invalid type name");
            }

            tok = lex();
            if (tok.token != TOK_RH_BRACE)
            {
                throw parse_error("Missing closing brace");
            }
            instructions.push_back(Token(TOK_TYPE));
        }
        else
        {
            throw parse_error("Expected a $ operator");
        }
    }
    else
    {
        parse_literal();
        instructions.push_back(Token(TOK_EQUAL));
    }
}

/**
 * Parses a literal value such as a string, number or true/false
 */
void QueryParser::parse_literal()
{
    auto tok = lex();
    switch (tok.token)
    {
    case TOK_STRING_LITERAL:
    case TOK_NUMERIC_LITERAL:
    case TOK_BOOL_LITERAL:
        instructions.push_back(tok);
        break;
    default:
        throw parse_error("Expected either a comparison operator or a literal value");
    }
}

/**
 * A literal list has the form
 * [ <literal1>, <literal2>,...<literal_N> ]
 */
void QueryParser::parse_literal_list()
{
    auto tok = lex();
    if (tok.token != TOK_LH_BRACKET)
    {
        throw parse_error("Expected lh bracket");
    }

    do
    {
        parse_literal();

        tok = lex();
        if ((tok.token != TOK_RH_BRACKET) && (tok.token != TOK_COMMA))
        {
            throw parse_error("Unexpected token in literal list");
        }
    } while (tok.token != TOK_RH_BRACKET);
}

/**
 * A field name has the form
 * "field_name"
 * or
 * "field_name"."subfield_name"."subsubfield_name"
 */
/*
void QueryParser::parse_field_name() 
{

}
*/