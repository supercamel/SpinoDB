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


#ifndef QUERY_EXECUTOR_H
#define QUERY_EXECUTOR_H

#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <cmath>
#include "rapidjson/rapidjson.h"
#include "rapidjson/pointer.h"
#include "rapidjson/allocators.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "QueryWalker.h"



namespace Spino {
	typedef rapidjson::GenericDocument<rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::CrtAllocator> DocType;
	typedef rapidjson::GenericValue<rapidjson::UTF8<>, rapidjson::CrtAllocator> ValueType;
	typedef rapidjson::GenericPointer<ValueType> PointerType;


	enum VALUE_TYPES
	{
		TYPE_STRING,
		TYPE_NUMERIC,
		TYPE_BOOLEAN,
		TYPE_ARRAY,
		TYPE_OBJECT,
		TYPE_NULL,
		TYPE_UNDEFINED
	};

	class Value {
		public:
			bool operator<(const Value& other) const
			{
				if(type != other.type) {
					return type < other.type;
				}
				else {
					if(type == TYPE_NUMERIC) {
						return numeric < other.numeric;
					}
					else if(type == TYPE_STRING) {
						return str.compare(other.str) < 0;
					}	
				}
				return false;
			}

			bool operator>(const Value& other) const
			{
				if(type != other.type) {
					return type > other.type;
				}
				else {
					if(type == TYPE_NUMERIC) {
						return numeric > other.numeric;
					}
					else if(type == TYPE_STRING) {
						return str.compare(other.str) > 0;
					}	
				}
				return false;
			}

			bool operator==(const Value& other) const
			{
				if(type == other.type) {
					if(type == TYPE_NUMERIC) {
						if(fabs(numeric-other.numeric) < 0.000001) {
							return true;
						}	
					} 
					else if(type == TYPE_STRING) {
                        return str == other.str;
					}
					else if(type == TYPE_BOOLEAN) {
						return boolean == other.boolean;
					}
					else if(type == TYPE_NULL) {
						return true;
					}
					else if(type == TYPE_UNDEFINED) {
						return true;
					}
				}
				return false;
			}

			std::string str;
			double numeric;
			bool boolean;
			bool null;
			bool undefined;
			uint32_t type;
	};


	class QueryExecutor : public QueryWalker {
		public:
			QueryExecutor() {
				doc = nullptr;
				stack_ptr = 0;
				stack.resize(100);
			}

			QueryExecutor(const ValueType* j) : doc(j) {
				stack_ptr = 0;
				stack.resize(10);
			}

			void set_json(const ValueType* j) {
				doc = j;
			}

			bool resolve(std::shared_ptr<class QueryNode> c);
			void Visit(class NumericValue* f);
			void Visit(class StringValue* s);
			void Visit(class BoolValue* b);
			void Visit(class RegexNode* rn);
			void Visit(class Field* f);
			void Visit(class LogicalExpression* l);
			void Visit(class List* l);
			void Visit(class Operator* a);
			void Visit(class BasicFieldComparison* c);
		private:
			const ValueType* doc;

			std::vector<Value> stack;
			uint32_t stack_ptr;
	};

}


#endif


