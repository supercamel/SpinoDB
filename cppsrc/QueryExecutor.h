#ifndef QUERY_EXECUTOR_H
#define QUERY_EXECUTOR_H

#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include "rapidjson/rapidjson.h"
#include "rapidjson/pointer.h"
#include "rapidjson/allocators.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"


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
						if(str == other.str) {
							return true;
						}
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


	class QueryExecutor {
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


