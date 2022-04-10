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


#ifndef QUERY_NODES_H
#define QUERY_NODES_H

#include "QueryExecutor.h"

namespace Spino {
	// base node type
	class QueryNode {
		public:
			virtual ~QueryNode() { }
			virtual void Accept(QueryWalker* t) = 0;
	};

	// represents a numeric value
	class NumericValue: public QueryNode {
		public:
			double value;
			virtual void Accept(QueryWalker* t) {
				t->Visit(this);
			}
	};

	// represents a string value
	class StringValue: public QueryNode {
		public: 
			std::string value;
			virtual void Accept(QueryWalker* t) {
				t->Visit(this);
			}
	};

	// represents a boolean value
	class BoolValue: public QueryNode {
		public:
			bool value;
			virtual void Accept(QueryWalker* t) {
				t->Visit(this);
			}
	};


	class RegexNode: public QueryNode {
		public:
			std::regex base_regex;

			virtual void Accept(QueryWalker* t) {
				t->Visit(this);
			}
	};


	// contains a json pointer to the field and the operation to perform on it
	// example: field -> operation (equal) -> numericValue (10)
	// loads the field, checks if it is equal to 10
	class Field: public QueryNode {
		public:
			std::shared_ptr<Operator> operation;
			PointerType jp;
            std::string path;

			virtual void Accept(QueryWalker* t) {
				t->Visit(this);
			}
	};

	// a logical expression such as AND or OR
	// op is the expression and may be  TOK_AND, TOK_OR
	// fields are the list of fields (with their respective operations) to load
	class LogicalExpression: public QueryNode {
		public:
			int op;
			std::vector<std::shared_ptr<QueryNode>> fields;

			virtual void Accept(QueryWalker* t) {
				t->Visit(this);
			}
	};

	// a list of query nodes
	class List: public QueryNode {
		public:
			std::vector<std::shared_ptr<QueryNode>> list;

			virtual void Accept(QueryWalker* t) {
				t->Visit(this);
			}
	};

	// an operator
	// op may be $eq, $ne, $gt, $lt, $in, $nin, $exists, $type
	// cmp is the node to perform the operation on
	// an operator always leaves a true/false on top of the stack
	class Operator: public QueryNode {
		public:
			int op;
			std::shared_ptr<QueryNode> cmp;

			virtual void Accept(QueryWalker* t) {
				t->Visit(this);
			}
	};

	class BasicFieldComparison: public QueryNode {
		public:
			std::string field_name;
			PointerType jp;
			Value v;

			virtual void Accept(QueryWalker* t) {
				t->Visit(this);
			}
	};

}



#endif


