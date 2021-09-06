#ifndef QUERY_PARSER_H
#define QUERY_PARSER_H

#include <string>
#include <memory>
#include <exception>

#include "QueryNodes.h"
#include "QueryWalker.h"
using namespace std;

namespace Spino {

class parse_error: public exception
{
	public:
		parse_error() {
			msg = nullptr;
		}

		parse_error(std::string _msg) {
			msg = _msg;
		}

		virtual const char* what() const throw()
		{
			return msg.c_str(); 
		}

	private:
		std::string msg;

};


enum TOKEN {
	TOK_LH_BRACE,
	TOK_RH_BRACE,
	TOK_LH_BRACKET,
	TOK_RH_BRACKET,
	TOK_COLON,
	TOK_AND,
	TOK_OR,
	TOK_NOR,
	TOK_NOT,
	TOK_IN,
	TOK_NIN,
	TOK_EQUAL,
	TOK_NE,
	TOK_GREATER_THAN,
	TOK_LESS_THAN,
	TOK_FIELD_NAME,
	TOK_STRING_LITERAL,
	TOK_NUMERIC_LITERAL,
	TOK_BOOL_LITERAL,
	TOK_PERIOD,
	TOK_COMMA,
	TOK_EXISTS,
	TOK_ELEM_MATCH,
	TOK_TYPE,
	TOK_EOF
};


class Token
{
	public:
		Token(TOKEN t, std::string txt) {
			token = t;
			raw = txt;
		}

		TOKEN token;
		std::string raw;
};

/**
 * QueryParser parses the query into a Query Tree
 **/

class QueryParser {
	public:
		QueryParser(const char* text);

		std::shared_ptr<QueryNode> parse_expression();
	private:
		const char* query_string;
		uint32_t cursor = 0;

		inline char curc() const {
			return query_string[cursor];
		}

		inline char next() {
			return query_string[cursor++];
		}

		std::string read_identifier();
		std::string read_string_literal();
		std::string read_numeric_literal();
		Token peek();
		Token lex();

		std::shared_ptr<Operator> parse_operator_expression();
		std::shared_ptr<QueryNode> parse_literal();
		std::vector<std::shared_ptr<QueryNode>> parse_literal_list();
};

}


#endif


