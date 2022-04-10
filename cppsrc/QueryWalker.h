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


#ifndef QUERY_WALKER_H
#define QUERY_WALKER_H

namespace Spino {
    class QueryWalker {
        public:
            virtual ~QueryWalker() { }
            virtual void Visit(class NumericValue* f) = 0;
            virtual void Visit(class StringValue* s) = 0;
            virtual void Visit(class BoolValue* b) = 0;
            virtual void Visit(class RegexNode* rn) = 0;
            virtual void Visit(class Field* f) = 0;
            virtual void Visit(class LogicalExpression* l) = 0;
            virtual void Visit(class List* l) = 0;
            virtual void Visit(class Operator* a) = 0;
            virtual void Visit(class BasicFieldComparison* c) = 0;
    };
}

#endif


