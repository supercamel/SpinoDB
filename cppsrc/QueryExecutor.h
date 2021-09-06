#ifndef QUERY_EXECUTOR_H
#define QUERY_EXECUTOR_H


#include "QueryWalker.h"
#include "QueryNodes.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/pointer.h"

namespace Spino {

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


	class QueryExecutor: public QueryWalker {
		public:
			QueryExecutor() {
				doc = nullptr;
				stack_ptr = 0;
				stack.resize(100);
			}

			QueryExecutor(const ValueType* j) : doc(j) {
				stack_ptr = 0;
				stack.resize(100);
			}

			void set_json(const ValueType* j) {
				doc = j;
			}
			bool resolve(std::shared_ptr<QueryNode> c);
			void Visit(NumericValue* f);
			void Visit(StringValue* s);
			void Visit(BoolValue* b);
			void Visit(Field* f);
			void Visit(LogicalExpression* l);
			void Visit(List* l);
			void Visit(Operator* a);
		private:
			const ValueType* doc;

			std::vector<Value> stack;
			uint32_t stack_ptr;
	};

}


#endif


