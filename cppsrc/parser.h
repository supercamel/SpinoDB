#ifndef SPINO_JSON_PARSER_H
#define SPINO_JSON_PARSER_H

#include "dom_node.h"
#include "rapidjson/reader.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

namespace Spino
{
    class Parser : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, Parser>
    {
    public:
        Parser()
        {
        }

        ~Parser()
        {
        }

        bool Null()
        {
            //DomNode* node = new DomNode();
            DomNode* node = dom_node_allocator.make();
            node->set_null();
            stack.push_back(node);
            return true;
        }

        bool Bool(bool b)
        {
            //DomNode* node = new DomNode();
            DomNode* node = dom_node_allocator.make();
            node->set_bool(b);
            stack.push_back(node);
            return true;
        }

        bool Int(int i)
        {
            //DomNode* node = new DomNode();
            DomNode* node = dom_node_allocator.make();
            node->set_int(i);
            stack.push_back(node);
            return true;
        }

        bool Uint(unsigned u)
        {
            //DomNode* node = new DomNode();
            DomNode* node = dom_node_allocator.make();
            node->set_uint(u);
            stack.push_back(node);
            return true;
        }

        bool Int64(int64_t i)
        {
            //DomNode* node = new DomNode();
            DomNode* node = dom_node_allocator.make();
            node->set_int(i);
            stack.push_back(node);
            return true;
        }

        bool Uint64(uint64_t u)
        {
            //DomNode* node = new DomNode();
            DomNode* node = dom_node_allocator.make();
            node->set_uint(u);
            stack.push_back(node);
            return true;
        }

        bool Double(double d)
        {
            //DomNode* node = new DomNode();
            DomNode* node = dom_node_allocator.make();
            node->set_double(d);
            stack.push_back(node);
            return true;
        }

        bool String(const char *str, rapidjson::SizeType length, bool copy)
        {
            //DomNode* node = new DomNode();
            DomNode* node = dom_node_allocator.make();
            node->set_string(str, length, copy);
            stack.push_back(node);
            return true;
        }

        bool StartObject()
        {
            //DomNode* node = new DomNode();
            DomNode* node = dom_node_allocator.make();
            node->set_object();
            stack.push_back(node);
            return true;
        }

        bool Key(const char *str, rapidjson::SizeType length, bool copy)
        {
            //DomNode* node = new DomNode();
            DomNode* node = dom_node_allocator.make();
            node->set_string(str, length, copy);
            stack.push_back(node);
            return true;
        }

        bool EndObject(rapidjson::SizeType memberCount)
        {
            size_t stack_len = stack.size();
            size_t obj_pos = stack_len - memberCount * 2 - 1;
            DomNode* obj = stack[obj_pos];
            for (size_t i = obj_pos + 1; i < stack_len; i += 2)
            {
                if(stack[i]->is_string() == false)
                {
                    return false;
                }
                obj->add_member(stack[i]->get_string(), stack[i+1]);
                dom_node_allocator.delete_object(stack[i]);
            } 

            stack.resize(obj_pos + 1);

            return true;
        }

        bool StartArray()
        {
            //DomNode* node = new DomNode();
            DomNode* node = dom_node_allocator.make();
            node->set_array();
            stack.push_back(node);
            return true;
        }

        bool EndArray(rapidjson::SizeType elementCount)
        {
            size_t stack_len = stack.size();
            size_t arr_pos = stack_len - elementCount - 1;
            DomNode* arr = stack[arr_pos];
            for (size_t i = arr_pos + 1; i < stack_len; i++)
            {
                arr->push_back(stack[i]);
            }

            stack.resize(arr_pos + 1);

            return true;
        }

        // must free the result with 
        // dom_node_allocator.delete_object(result);
        DomNode *parse(const std::string &str)
        {
            rapidjson::Reader reader;
            rapidjson::StringStream ss(str.c_str());
            reader.Parse(ss, *this);
            if(reader.HasParseError())
            {
                return nullptr;
            }
            return stack[0];
        }

    private:
        std::vector<DomNode *> stack;
    };
}

#endif
