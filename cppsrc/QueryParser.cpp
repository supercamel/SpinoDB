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


#include "QueryParser.h"

#include <sstream>
#include <set>

using namespace std;

using namespace Spino;

QueryParser::QueryParser(const char* query) {
    query_string = query;
    cursor = 0;
}

Token QueryParser::peek() {
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
Token QueryParser::lex() {
    // while there are characters left to consume
    while(curc() != '\0') {
        //get the current character
        switch(curc()) {
            // chew up white space
            case '\r':
            case '\n':
            case ' ':
            case '\t':
                next();
                continue;
            case '{':
                next();
                return Token(TOK_LH_BRACE, "{");
            case '}':
                next();
                return Token(TOK_RH_BRACE, "}");
            case '[':
                next();
                return Token(TOK_LH_BRACKET, "[");
            case ']':
                next();
                return Token(TOK_RH_BRACKET, "]");
            case ':':
                next();
                return Token(TOK_COLON, ":");
            case '.':
                next();
                return Token(TOK_PERIOD, ".");
            case ',':
                next();
                return Token(TOK_COMMA, ",");
            case '"':
                return Token(TOK_STRING_LITERAL, read_string_literal());
            case '$':
                next();
                auto op = "$" + read_identifier();
                if(op == "$eq") {
                    return Token(TOK_EQUAL, op);
                }
                else if(op == "$gt") {
                    return Token(TOK_GREATER_THAN, op);
                }
                else if(op == "$lt") {
                    return Token(TOK_LESS_THAN, op);
                }
                else if(op == "$and") {
                    return Token(TOK_AND, op);
                }
                else if(op == "$or") {
                    return Token(TOK_OR, op);
                }
                else if(op == "$ne") {
                    return Token(TOK_NE, op);
                }
                else if(op == "$nin") {
                    return Token(TOK_NIN, op);
                }
                else if(op == "$not") {
                    return Token(TOK_NOT, op);
                }
                else if(op == "$nor") {
                    return Token(TOK_NOR, op);
                }
                else if(op == "$in") {
                    return Token(TOK_IN, op);
                }
                else if(op == "$elemMatch") {
                    return Token(TOK_ELEM_MATCH, op);
                }
                else if(op == "$exists") {
                    return Token(TOK_EXISTS, op);
                }
                else if(op == "$type") {
                    return Token(TOK_TYPE, op);
                }
                else if(op == "$startsWith") {
                    return Token(TOK_STARTS_WITH, op);
                }
                else if(op == "$regex") {
                    return Token(TOK_REGEX, op);
                }
                else if(op == "$range") {
                    return Token(TOK_RANGE, op);
                }
                else {
                    throw parse_error("Unknown $ operator");
                }
                break;
        }
        // if the current character is alphabetic (a-z, A-Z)
        if(isalpha(curc())) {
            // read an identifier
            auto ident = read_identifier();
            // if its true/false then it's a boolean literal
            if((ident == "true") || (ident == "false")) {
                return Token(TOK_BOOL_LITERAL, ident);
            }
            // otherwise it's a field name
            return Token(TOK_FIELD_NAME, ident);
        }
        // if it's a number
        else if(isdigit(curc()) || (curc() == '-')) {
            // read a numeric literal
            return Token(TOK_NUMERIC_LITERAL, read_numeric_literal());
        }
        // else throw an error because dunno what this is supposed to be
        throw parse_error("Expected character " + curc());
    }
    return Token(TOK_EOF, "");
}

// read identifier is usually for reading a field name
// like 'person.name'
std::string QueryParser::read_identifier() {
    std::string s;
    s += curc();
    while(curc() != '\0') {
        next();
        if((!isalpha(curc())) && (!isdigit(curc())) && (curc() != '_') && (curc() != '.')) {
            return s;
        }
        s += curc();
    }
    throw parse_error("Unexpected end of query");
    return s;
}

// read_string_literal() reads a string . . 
// such a "Hello world"
std::string QueryParser::read_string_literal() {
    std::string s;
    next();
    while(curc() != '\0') {
        char c = curc();
        if(c == '\\') {
            next();
            c = curc();
            if(c == '\\') {
                s += "\\";
            } 
            else {
                s += "\\";
                s += c;
            }
        } 
        else if(c == '"') {
            next();
            return s;
        }
        else {
            s += c;
        }
        next();
    }
    throw parse_error("Unexpected end of query");
    return s;
}

// read_numeric_literal 
// returns a number in string format
// e.g. "12.5"
std::string QueryParser::read_numeric_literal() {
    std::string s;
    bool period_spotted = false;
    if(curc() == '-') {
        s = "-";
        next();
        if(!isdigit(curc())) {
            throw parse_error("Expected '-' character");
        }
    }

    while(curc() != '\0') {
        char c = curc();
        if(!isdigit(curc())) {
            if(curc() == '.') {
                if(period_spotted) {
                    return s;
                } else {
                    period_spotted = true;
                    s += c;
                }
            } else {
                return s;
            }
        } else {
            s += c;
        }
        next();
    }

    throw parse_error("Unexpected end of query while parsing numeric literal");
    return s;
}

std::shared_ptr<BasicFieldComparison> QueryParser::parse_basic_comparison() {
    if(lex().token == TOK_LH_BRACE) {
        auto tok = lex();
        // if the token is a field, parse rhs
        if(tok.token == TOK_FIELD_NAME) {
            std::string field_name = tok.raw;
            stringstream ss(tok.raw);
            string intermediate;
            string ptr;
            while(getline(ss, intermediate, '.')) {
                ptr += "/" + intermediate;
            }

            tok = lex();
            if(tok.token != TOK_COLON) {
                return nullptr;
            }

            tok = peek();
            if((tok.token == TOK_STRING_LITERAL) || (tok.token == TOK_NUMERIC_LITERAL)) {
                tok = lex();
                auto cmp = make_shared<BasicFieldComparison>();
                cmp->field_name = field_name;
                cmp->jp = PointerType(ptr.c_str());

                if(tok.token == TOK_STRING_LITERAL) {
                    cmp->v.type = TYPE_STRING;
                    cmp->v.str = tok.raw;
                } else {
                    cmp->v.type = TYPE_NUMERIC;
                    cmp->v.numeric = std::stof(tok.raw);
                }

                if(lex().token == TOK_RH_BRACE) {
                    return cmp;
                }
            } 
        }
    }
    return nullptr;
}

/* an expression can have the form
 * { <field_name>: <operator_expression> }
 * { $and/$or: { [ <expression1>, <expression2>,...<expressionN> ] }
 * { $not: { <expression> }}
 */
std::shared_ptr<QueryNode> QueryParser::parse_expression() {
    if(lex().token == TOK_LH_BRACE) {
        auto tok = lex();
        // if the token is a field, parse rhs
        if(tok.token == TOK_FIELD_NAME) {
            auto f = make_shared<Field>();
            f->path = tok.raw;
            stringstream ss(tok.raw);
            string intermediate;
            string ptr;
            while(getline(ss, intermediate, '.')) {
                ptr += "/" + intermediate;
            }
            f->jp = PointerType(ptr.c_str());

            tok = lex();
            if(tok.token != TOK_COLON) {
                throw parse_error("Expected a colon after identifier");
            }

            tok = peek();
            if((tok.token == TOK_STRING_LITERAL) || (tok.token == TOK_NUMERIC_LITERAL)) {
                tok = lex();
                auto cmp = make_shared<BasicFieldComparison>();
                cmp->field_name = f->path;
                cmp->jp = PointerType(ptr.c_str());

                if(tok.token == TOK_STRING_LITERAL) {
                    cmp->v.type = TYPE_STRING;
                    cmp->v.str = tok.raw;
                } else {
                    cmp->v.type = TYPE_NUMERIC;
                    cmp->v.numeric = std::stof(tok.raw);
                }

                if(lex().token != TOK_RH_BRACE) {
                    throw parse_error("Missing closing brace after basic field comparison");
                }

                return cmp;
            } else {
                f->operation = parse_operator_expression();
            }

            tok = lex();
            if(tok.token != TOK_RH_BRACE) {
                throw parse_error("Missing closing brace 5");	
            }
            return f;
        }
        else if((tok.token == TOK_AND) || (tok.token == TOK_OR)) {
            auto a = make_shared<LogicalExpression>();
            a->op = tok.token;

            tok = lex();
            if(tok.token != TOK_COLON) {
                throw parse_error("Missing colon after " + tok.raw);
            }

            tok = lex();
            if(tok.token != TOK_LH_BRACKET) {
                throw parse_error("Expected lh bracket");
            }

            do {
                a->fields.push_back(parse_expression());

                tok = lex();
                if((tok.token != TOK_RH_BRACKET) && (tok.token != TOK_COMMA)) {
                    throw parse_error("Unexpected token in list. " + tok.raw);
                }
            } while(tok.token != TOK_RH_BRACKET);

            if(lex().token != TOK_RH_BRACE) {
                throw parse_error("Missing closing brace 4");
            }

            return a;
        }
        else if(tok.token == TOK_NOT) {
            auto a = make_shared<Operator>();
            a->op = TOK_NOT;

            if(lex().token != TOK_COLON) {
                throw parse_error("Expected colon after $not");
            }

            a->cmp = parse_expression();

            if(lex().token != TOK_RH_BRACE) {
                throw parse_error("Missing closing brace 3");
            }
            return a;
        }
    }
    else {
        throw parse_error("Missing opening brace");
    }

    return nullptr;
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
std::shared_ptr<Operator> QueryParser::parse_operator_expression() {
    auto ret = make_shared<Operator>();

    auto tok = peek();
    if(tok.token == TOK_LH_BRACE) {
        tok = lex();
        tok = lex();
        if((tok.token == TOK_EQUAL) ||
                (tok.token == TOK_NE) ||
                (tok.token == TOK_GREATER_THAN) ||
                (tok.token == TOK_LESS_THAN) ||
                (tok.token == TOK_STARTS_WITH)) {
            ret->op = tok.token;

            tok = lex();
            if(tok.token != TOK_COLON) {
                throw parse_error("Expected : after " + tok.raw);
            }

            ret->cmp = parse_literal();

            tok = lex();
            if(tok.token != TOK_RH_BRACE) {
                throw parse_error("Missing closing brace 2");
            }
        }
        else if(tok.token == TOK_RANGE) {
            ret->op = tok.token;
            tok = lex();
            if(tok.token != TOK_COLON) {
                throw parse_error("Expected : after $range");
            }

            tok = lex();
            if(tok.token != TOK_LH_BRACKET) {
                throw parse_error("Expected LH bracket after $range:");
            }
            
            auto l = make_shared<List>();

            tok = peek();
            if(tok.token != TOK_NUMERIC_LITERAL) {
                throw parse_error("Expected numeric literal as lower bound of $range expression");
            }

            l->list.push_back(parse_literal());

            tok = lex();
            if(tok.token != TOK_COMMA) {
                throw parse_error("Expected comma after lower bound");
            }

            tok = peek();
            if(tok.token != TOK_NUMERIC_LITERAL) {
                throw parse_error("Expected numeric literal as upper bound of $range expression");
            }

            l->list.push_back(parse_literal());
            ret->cmp = l;

            tok = lex();
            if(tok.token != TOK_RH_BRACKET) {
                throw parse_error("Expected rh bracket to close $range list");
            }

            tok = lex();
            if(tok.token != TOK_RH_BRACE) {
                throw parse_error("Missing closing brace");
            }
        }
        else if(tok.token == TOK_REGEX) {
            ret->op = tok.token;

            tok = lex();
            if(tok.token != TOK_COLON) {
                throw parse_error("Expected : after " + tok.raw);
            }

            tok = lex();
            if(tok.token != TOK_STRING_LITERAL) {
                throw parse_error("Expected string as regex parameter");
            }

            auto rn = make_shared<RegexNode>();

            try {
                if(tok.raw.rfind("(?i)", 0) == 0) {
                    rn->base_regex = std::regex(tok.raw.substr(4, std::string::npos), std::regex_constants::icase);
                } 
                else {
                    rn->base_regex = std::regex(tok.raw);
                }
            }
            catch(std::regex_error& err) {
                std::string errmsg = "Invalid regex: ";
                errmsg += err.what();
                throw parse_error(errmsg);
            }

            ret->cmp = rn;

            tok = lex();
            if(tok.token != TOK_RH_BRACE) {
                throw parse_error("Missing closing brace 2");
            }
        }
        else if((tok.token == TOK_IN) || (tok.token == TOK_NIN)) {
            ret->op = tok.token;

            tok = lex();
            if(tok.token != TOK_COLON) {
                throw parse_error("Expected colon after $in");
            }

            auto l = make_shared<List>();
            l->list = parse_literal_list();
            ret->cmp = l;

            tok = lex();
            if(tok.token != TOK_RH_BRACE) {
                throw parse_error("Missing closing brace");
            }
        }
        else if(tok.token == TOK_EXISTS) {
            ret->op = tok.token;
            tok = lex();
            if(tok.token != TOK_COLON) {
                throw parse_error("Expected colon after $exists");
            }

            auto b = make_shared<BoolValue>();
            tok = lex();
            if(tok.token != TOK_BOOL_LITERAL) {
                throw parse_error("Expected true/false literal after $exists");
            }

            if(tok.raw == "true") {
                b->value = true;
            }
            else {
                b->value = false;
            }

            ret->cmp = b;

            tok = lex();
            if(tok.token != TOK_RH_BRACE) {
                throw parse_error("Missing closing brace");
            }
        }
        else if(tok.token == TOK_TYPE) {
            ret->op = tok.token;
            tok = lex();
            if(tok.token != TOK_COLON) {
                throw parse_error("Expected colon after $type");
            }

            tok = lex();
            if(tok.token != TOK_FIELD_NAME) {
                throw parse_error("Expected type name after $type");
            }

            std::set<std::string> type_names = {
                "number",
                "string",
                "bool",
                "array",
                "object"
            };

            if(type_names.find(tok.raw) != type_names.end()) {
                auto b = make_shared<StringValue>();
                b->value = tok.raw;
                ret->cmp = b;
            }
            else {
                throw parse_error("Invalid type name");	
            }

            tok = lex();
            if(tok.token != TOK_RH_BRACE) {
                throw parse_error("Missing closing brace");
            }

        }
        else {
            throw parse_error("Expected a $ operator");
        }

    }
    else {
        ret->op = TOK_EQUAL;
        ret->cmp = parse_literal();
    }
    return ret;
}

/**
 * Parses a literal value such as a string, number or true/false
 */
std::shared_ptr<QueryNode> QueryParser::parse_literal() {
    auto tok = lex();
    if(tok.token == TOK_STRING_LITERAL) {
        auto r = make_shared<StringValue>();
        r->value = tok.raw;
        return r;
    }
    else if(tok.token == TOK_NUMERIC_LITERAL) {
        auto r = make_shared<NumericValue>();
        r->value = std::stof(tok.raw);
        return r;
    }
    else if(tok.token == TOK_BOOL_LITERAL) {
        auto r = make_shared<BoolValue>();
        if(tok.raw == "true") {
            r->value = true;
        } 
        else {
            r->value = false;
        }
        return r;
    }
    throw parse_error("Expected either a comparison operator or a literal value");
}


/**
 * A literal list has the form
 * [ <literal1>, <literal2>,...<literal_N> ]
 */
std::vector<std::shared_ptr<QueryNode>> QueryParser::parse_literal_list() {
    std::vector<std::shared_ptr<QueryNode>> ret;
    auto tok = lex();
    if(tok.token != TOK_LH_BRACKET) {
        throw parse_error("Expected lh bracket");
    }

    do {
        ret.push_back(parse_literal());

        tok = lex();
        if((tok.token != TOK_RH_BRACKET) && (tok.token != TOK_COMMA)) {
            throw parse_error("Unexpected token in literal list");
        }
    } while(tok.token != TOK_RH_BRACKET);

    return ret;
}
