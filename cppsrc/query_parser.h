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

#ifndef QUERY_PARSER_H
#define QUERY_PARSER_H

#include <string>
#include <list>
#include <memory>
#include <exception>
#include <regex>
#include "dom_node.h"

using namespace std;

namespace Spino
{

    typedef std::list<DomNode*> ContainerType;
	typedef ContainerType::iterator NodeListIterator;
	typedef std::multimap<DomView, NodeListIterator, std::less<DomView>> IndexType;
	typedef IndexType::const_iterator IndexIterator;
	typedef std::pair<IndexIterator, IndexIterator> IndexIteratorRange;

    class Index {
        public:
            Index(std::string field_name) : field_name(field_name) {}
            std::string field_name;
            IndexType index;
    };


	class parse_error : public exception
	{
	public:
		parse_error()
		{
			msg = nullptr;
		}

		parse_error(std::string _msg)
		{
			msg = _msg;
		}

		virtual const char *what() const throw()
		{
			return msg.c_str();
		}

	private:
		std::string msg;
	};

	enum TOKEN
	{
		TOK_LH_BRACE,
		TOK_RH_BRACE,
		TOK_LH_BRACKET,
		TOK_RH_BRACKET,
		TOK_COLON,
		TOK_AND,
		TOK_OR,
		TOK_NOT,
		TOK_IN,
		TOK_NIN,
		TOK_EQUAL,
		TOK_NE,
		TOK_GREATER_THAN,
		TOK_LESS_THAN,
		TOK_RANGE,
		TOK_STARTS_WITH,
		TOK_REGEX,
		TOK_FIELD_NAME,
		TOK_STRING_LITERAL,
		TOK_NUMERIC_LITERAL,
		TOK_BOOL_LITERAL,
		TOK_PERIOD,
		TOK_COMMA,
		TOK_EXISTS,
		TOK_TYPE,
		TOK_EOF
	};

	inline const char *token_to_string(TOKEN tok)
	{
		switch (tok)
		{
		case TOK_AND:
			return "and";
		case TOK_BOOL_LITERAL:
			return "bool";
		case TOK_EQUAL:
			return "equal";
		case TOK_EXISTS:
			return "exists";
		case TOK_FIELD_NAME:
			return "field";
		case TOK_GREATER_THAN:
			return "greater than";
		case TOK_IN:
			return "in";
		case TOK_LESS_THAN:
			return "less than";
		case TOK_NE:
			return "not equal";
		case TOK_NIN:
			return "not in";
		case TOK_NOT:
			return "not";
		case TOK_NUMERIC_LITERAL:
			return "number";
		case TOK_OR:
			return "or";
		case TOK_RANGE:
			return "range";
		case TOK_REGEX:
			return "regex";
		case TOK_STARTS_WITH:
			return "starts with";
		case TOK_STRING_LITERAL:
			return "string";
		case TOK_TYPE:
			return "type";
		default:
			return "grammar";
		}
	}

	class Token
	{
	public:
		Token(TOKEN t)
		{
			token = t;
			regex = nullptr;
		}

		Token(TOKEN t, const char *txt, size_t _len)
		{
			token = t;
			raw = txt;
			len = _len;
			regex = nullptr;
		}

		TOKEN token;
		const char* raw;
		size_t len;
		double value;
		shared_ptr<std::regex> regex;
	};

	/**
	 * QueryParser parses the query into an instruction list
	 * It also isolates the query results to a range of an index
	 **/

	class QueryParser
	{
	public:
		QueryParser(const std::vector<Index> &indicies, const char *text);

		std::vector<Token>* parse_query(IndexIteratorRange &range);

	private:
		std::string query_string;
		uint32_t cursor = 0;

		const char *cursor_ptr()
		{
			return &query_string[cursor];
		}

		inline char curc() const
		{
			return query_string[cursor];
		}

		inline char next()
		{
			return query_string[cursor++];
		}

		size_t read_identifier();
		size_t read_string_literal();
		double read_numeric_literal();
		Token peek();
		inline Token lex();

		void parse_expression();
		void parse_basic_comparison();
		void parse_operator_expression();
		void parse_literal();
		void parse_literal_list();

		DomNode* token_literal_to_node(Token t);

		const std::vector<Index> &indicies;
		IndexIteratorRange range;
		bool index_resolved;

		std::vector<Token> instructions;
	};

}

#endif
