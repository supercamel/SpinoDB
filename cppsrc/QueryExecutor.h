#ifndef QUERY_EXECUTOR_H
#define QUERY_EXECUTOR_H

#include <string>
#include <vector>
#include <memory>
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

	struct Value {
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


