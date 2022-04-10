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


#ifndef INDEX_RESOLVER_H 
#define INDEX_RESOLVER_H 

#include "QueryExecutor.h"
#include "Cursor.h"
#include "Collection.h"

/**
 * The IndexResolver inspects the query and finds the best index to use.
 */
namespace Spino {
    class IndexResolver : public QueryWalker {
        public:
            IndexResolver(const std::vector<Index*>& indices) : indices(indices)
            {
                range = nullptr;
                exit = false;
            }

            ~IndexResolver() { }

            bool resolve(std::shared_ptr<class QueryNode> c, std::shared_ptr<IndexIteratorRange> range);
            void Visit(NumericValue* f);
			void Visit(StringValue* s);
			void Visit(BoolValue* b);
			void Visit(RegexNode* rn);
			void Visit(Field* f);
			void Visit(LogicalExpression* l);
			void Visit(List* l);
			void Visit(Operator* a);
			void Visit(BasicFieldComparison* c);

        private:
            bool exit;
            bool drop_node;
            std::shared_ptr<IndexIteratorRange> range;
            const std::vector<Index*>& indices;

            Value top;
    };
}



#endif

