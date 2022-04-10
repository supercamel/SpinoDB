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


#include "IndexResolver.h"
#include "QueryNodes.h"

#include <iostream>
using namespace std;

namespace Spino {

    bool IndexResolver::resolve(
            std::shared_ptr<QueryNode> c,
            shared_ptr<IndexIteratorRange> _range) 
    {
        exit = false;
        drop_node = false;
        range = _range;
        c->Accept(this);
        if(drop_node) {
            c = nullptr;
        }
        return exit;
    }


    void IndexResolver::Visit(NumericValue* f) {
        top.numeric = f->value;
        top.type = TYPE_NUMERIC;
    }

    void IndexResolver::Visit(StringValue* s) {
        top.str = s->value;
        top.type = TYPE_STRING;
    }

    void IndexResolver::Visit(BoolValue* b) {
        top.boolean = b->value;
        top.type = TYPE_BOOLEAN;
    }

    void IndexResolver::Visit(RegexNode* rn) {

    }

    void IndexResolver::Visit(Field* f) {
        if(exit) {
            return;
        }

        if(f->operation->op == TOK_EQUAL) {
            for(auto& idx : indices) {
                if(f->path == idx->field_name) {
                    top.type = TYPE_NULL;
                    f->operation->Accept(this);
                    if((top.type == TYPE_STRING) || (top.type == TYPE_STRING)) {
                        *range = idx->index.equal_range(top);
                        exit = true;
                        drop_node = true;
                        return;
                    }
                }
            }
        }
    }

    void IndexResolver::Visit(LogicalExpression* l) {
        auto it = l->fields.begin();
        while(it != l->fields.end()) {
            if(exit) {
                return;
            }

            auto f = *it;
            f->Accept(this);
            if(drop_node) {
                it = l->fields.erase(it);
                drop_node = false;
            }
            else {
                it++;
            }
        }
    }


    void IndexResolver::Visit(List* l) {

    }

    void IndexResolver::Visit(Operator* a) {
        a->cmp->Accept(this);
    }

    void IndexResolver::Visit(BasicFieldComparison* c) {
        for(auto idx : indices) {
            if(c->field_name == idx->field_name) {
                auto r = idx->index.equal_range(c->v);
                *range = r;
                exit = true;
                break;
            }
        }
    }


}


