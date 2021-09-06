#ifndef QUERY_PRINTER_H
#define QUERY_PRINTER_H

/**
 * The QueryPrinter walks the query tree and prints it to the terminal.
 *
 * This is not used except to visual the tree (for development reasons).
 *
 */

/*
#include "QueryWalker.h"
#include "QueryNodes.h"
#include <iostream>

namespace Spino {

	class QueryPrinter: public QueryWalker {
		public:
			void Visit(NumericValue* f);
			void Visit(StringValue* s);
			void Visit(BoolValue* b);
			void Visit(Field* f);
			void Visit(LogicalExpression* l);
			void Visit(List* l);
		 	void Visit(Operator* a);
		private:
			void print_indent() {
				for(int i = 0; i < indent; i++) {
					std::cout << "    ";
				}
			}

			int indent = 0;
	};

}
*/


#endif


