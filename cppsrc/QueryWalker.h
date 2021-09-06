#ifndef QUERY_WALKER_H
#define QUERY_WALKER_H

namespace Spino {
	class QueryWalker {
		public:
			virtual void Visit(class NumericValue* f) { }
			virtual void Visit(class StringValue* s) { }
			virtual void Visit(class BoolValue* b) { }
			virtual void Visit(class Field* f) { }
			virtual void Visit(class LogicalExpression* l) { }
			virtual void Visit(class List* a) { }
		 	virtual void Visit(class Operator* a) { }	
	};
}


#endif


