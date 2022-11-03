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

#include "query_executor.h"
#include "query_parser.h"

#include <regex>

#include <iostream>
using namespace std;

namespace Spino
{
    QueryExecutor::QueryExecutor()
    {
    }

    void QueryExecutor::set_instructions(const std::vector<Token>* i)
    {
        tokens = *i;
    }

    bool QueryExecutor::execute_query(DomNode *document)
    {
        if (tokens.size() == 0)
        {
            return true;
        }

/*
        while(stack.size())
        {
            DomNode n = stack.back();
            stack.pop_back();
            n.destroy();
        }
        */

        for (auto tok : tokens)
        {
            switch (tok.token)
            {
            case TOK_STRING_LITERAL:
            {
                DomNode* d = dom_node_allocator.make();
                d->set_string(tok.raw, tok.len, true);
                stack.push_back(d);
            }
            break;
            case TOK_NUMERIC_LITERAL:
            {
                DomNode* d = dom_node_allocator.make();
                d->set_double(tok.value);
                stack.push_back(d);
            }
            break;
            case TOK_BOOL_LITERAL:
            {
                DomNode* d = dom_node_allocator.make();
                d->set_bool(tok.raw[0] == 't');
                stack.push_back(d);
            }
            break;
            case TOK_FIELD_NAME:
            {
                std::string field_name = std::string(tok.raw, tok.len);
                if(document->has_member(field_name)) {
                    DomView tmp = document->get_member(field_name);
                    DomNode* d = dom_node_allocator.make();
                    d->copy(&tmp);
                    stack.push_back(d);
                }
                else {
                    DomNode* d = dom_node_allocator.make();
                    d->set_invalid();
                    stack.push_back(d);
                }
            }
            break;
            case TOK_EQUAL:
            {
                auto end = stack.end();
                DomNode* b = *(--end);
                DomNode* a = *(--end);

                if (*a == *b)
                {
                    a->set_bool(true);
                }
                else
                {
                    a->set_bool(false);
                }

                stack.pop_back();
                dom_node_allocator.delete_object(b);
            }
            break;
            case TOK_NE:
            {
                auto end = stack.end();
                DomNode* b = *--end;
                DomNode* a = *--end;

                if (*a == *b)
                {
                    a->set_bool(false);
                }
                else
                {
                    a->set_bool(true);
                }
                stack.pop_back();
                dom_node_allocator.delete_object(b);
            }
            break;
            case TOK_RANGE:
            {
                auto end = stack.end();

                DomNode* max = *--end;
                DomNode* min = *--end;
                DomNode* doc = *--end;

                DomNode* result = dom_node_allocator.make();
                if (!doc->is_numeric())
                {
                    result->set_bool(false);
                }
                else
                {
                    if ((*doc >= *min) && (*doc <= *max))
                    {
                        result->set_bool(true);
                    }
                    else
                    {
                        result->set_bool(false);
                    }
                }

                stack.erase(end, stack.end());
                dom_node_allocator.delete_object(doc);
                dom_node_allocator.delete_object(min);
                dom_node_allocator.delete_object(max);
                stack.push_back(result);
            }
            break;
            case TOK_GREATER_THAN:
            {
                DomNode* val = stack.back();
                stack.pop_back();

                DomNode* doc = stack.back();
                stack.pop_back();

                DomNode* result = dom_node_allocator.make();
                if (!doc->is_numeric())
                {
                    result->set_bool(false);
                }
                else
                {
                    if (*doc > *val)
                    {
                        result->set_bool(true);
                    }
                    else
                    {
                        result->set_bool(false);
                    }
                }
                dom_node_allocator.delete_object(doc);
                dom_node_allocator.delete_object(val);
                stack.push_back(result);
            }
            break;
            case TOK_LESS_THAN:
            {
                DomNode* val = stack.back();
                stack.pop_back();

                DomNode* doc = stack.back();
                stack.pop_back();

                DomNode* result = dom_node_allocator.make();
                if (!doc->is_numeric())
                {
                    result->set_bool(false);
                }
                else
                {
                    if (*doc < *val)
                    {
                        result->set_bool(true);
                    }
                    else
                    {
                        result->set_bool(false);
                    }
                }
                dom_node_allocator.delete_object(doc);
                dom_node_allocator.delete_object(val);
                stack.push_back(result);
            }
            break;
            case TOK_EXISTS:
            {
                DomNode* val = stack.back();
                stack.pop_back();

                DomNode* exists = stack.back();

                if (val->get_type() == DOM_NODE_TYPE_INVALID)
                {
                    exists->set_bool(!exists->get_bool());
                }

                dom_node_allocator.delete_object(val);
            }
            break;
            case TOK_AND:
            {
                DomNode* result = stack.back();
                int count = result->get_double();
                stack.pop_back();

                result->set_bool(true);
                while (--count)
                {
                    auto v = stack.back();
                    stack.pop_back();
                    if (v->get_bool() == false)
                    {
                        result->set_bool(false);
                    }
                    dom_node_allocator.delete_object(v);
                }
                stack.push_back(result);
            }
            break;
            case TOK_OR:
            {
                DomNode* result = stack.back();
                int count = result ->get_double();
                stack.pop_back();

                result->set_bool(false);
                while (--count)
                {
                    auto v = stack.back();
                    stack.pop_back();
                    if (v->get_bool() == true)
                    {
                        result->set_bool(true);
                    }
                    dom_node_allocator.delete_object(v);
                }
                stack.push_back(result);
            }
            break;
            case TOK_TYPE:
            {
                DomNode* type_str = stack.back();
                stack.pop_back();

                DomNode* field = stack.back();
                stack.pop_back();

                DomNode* result = dom_node_allocator.make();
                result->set_bool(false);
                if (strncmp(type_str->get_string(), "bool", 4) == 0)
                {
                    if (field->get_type() == DOM_NODE_TYPE_BOOL)
                    {
                        result->set_bool(true);
                    }
                }
                else if (strncmp(type_str->get_string(), "number", 6) == 0)
                {
                    if (field->is_numeric() && (field->get_type() != DOM_NODE_TYPE_BOOL))
                    {
                        result->set_bool(true);
                    }
                }
                else if (strncmp(type_str->get_string(), "string", 6) == 0)
                {
                    if (field->is_string())
                    {
                        result->set_bool(true);
                    }
                }
                else if (strncmp(type_str->get_string(), "array", 5) == 0)
                {
                    if (field->get_type() == DOM_NODE_TYPE_ARRAY)
                    {
                        result->set_bool(true);
                    }
                }
                else if (strncmp(type_str->get_string(), "object", 6) == 0)
                {
                    if (field->get_type() == DOM_NODE_TYPE_OBJECT)
                    {
                        result->set_bool(true);
                    }
                }
                else if (strncmp(type_str->get_string(),"null", 4) == 0)
                {
                    if (field->get_type() == DOM_NODE_TYPE_NULL)
                    {
                        result->set_bool(true);
                    }
                }
                dom_node_allocator.delete_object(type_str);
                dom_node_allocator.delete_object(field);
                stack.push_back(result);
            }
            break;
            case TOK_STARTS_WITH:
            {
                DomNode* val = stack.back();
                stack.pop_back();

                DomNode* field = stack.back();
                stack.pop_back();

                DomNode* result = dom_node_allocator.make();
                if (field->is_string())
                {
                    std::string field_string = field->get_string();
                    if (field_string.rfind(val->get_string(), 0) == 0)
                    {
                        result->set_bool(true);
                    }
                    else
                    {
                        result->set_bool(false);
                    }
                }
                else
                {
                    result->set_bool(false);
                }

                dom_node_allocator.free(field);
                dom_node_allocator.free(val);
                stack.push_back(result);
            }
            break;
            case TOK_IN:
            {
                DomNode *lenvar = stack.back();
                size_t length = lenvar->get_double();

                stack.pop_back();
                dom_node_allocator.delete_object(lenvar);

                DomNode* field = stack[stack.size()-(length+1)];
                DomNode* result = dom_node_allocator.make();

                result->set_bool(false);
                while (length--)
                {
                    DomNode* val = stack.back();
                    stack.pop_back();

                    if (*field == *val)
                    {
                        result->set_bool(true);
                    }
                    dom_node_allocator.delete_object(val);
                }

                stack.pop_back();
                dom_node_allocator.delete_object(field);

                stack.push_back(result);
            }
            break;
            case TOK_NIN:
            {
                DomNode* result = stack.back();
                size_t length = result->get_double();
                stack.pop_back();

                DomNode* field = stack[stack.size() - (length + 1)];

                result->set_bool(true);
                while (length--)
                {
                    DomNode* val = stack.back();
                    stack.pop_back();

                    if (*field == *val)
                    {
                        result->set_bool(false);
                    }
                    dom_node_allocator.delete_object(val);
                }

                stack.pop_back();
                dom_node_allocator.delete_object(field);
                stack.push_back(result);
            }
            break;
            case TOK_NOT:
            {
                DomNode* val = stack.back();
                val->set_bool(!val->get_bool());
            }
            break;
            case TOK_REGEX:
            {
                DomNode* field = stack.back();
                if (field->is_string())
                {
                    std::smatch base_match;
                    const std::regex reg = *tok.regex;
                    std::string str = field->get_string();
                    field->set_bool(std::regex_match(str, base_match, reg));
                }
                else
                {
                    field->set_bool(false);
                }
            }
            break;
            case TOK_RH_BRACE:
            case TOK_LH_BRACE:
            case TOK_LH_BRACKET:
            case TOK_RH_BRACKET:
            case TOK_COLON:
            case TOK_PERIOD:
            case TOK_COMMA:
            case TOK_EOF:
            break;
            }
        }

        bool result = stack.back()->get_bool();
        while(stack.size()) {
            dom_node_allocator.delete_object(stack.back());
            stack.pop_back();
        }
        return result;
    }

}
