#ifndef SPINO_JSON_PARSER_H
#define SPINO_JSON_PARSER_H

#include <exception>
#include "dom_node.h"
#include "rapidjson/reader.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/error/en.h"

namespace Spino
{
    class json_parse_error : public exception
    {
    public:
        json_parse_error()
        {
            msg = nullptr;
        }

        json_parse_error(std::string _msg)
        {
            msg = _msg;
        }

        virtual const char *what() const throw()
        {
            return msg.c_str();
        }

    private:
        std::string msg;
    };

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
            DomNode *node = dom_node_allocator.make();
            node->set_null();
            stack.push_back(node);
            return true;
        }

        bool Bool(bool b)
        {
            DomNode *node = dom_node_allocator.make();
            node->set_bool(b);
            stack.push_back(node);
            return true;
        }

        bool Int(int i)
        {
            DomNode *node = dom_node_allocator.make();
            node->set_int(i);
            stack.push_back(node);
            return true;
        }

        bool Uint(unsigned u)
        {
            DomNode *node = dom_node_allocator.make();
            node->set_uint(u);
            stack.push_back(node);
            return true;
        }

        bool Int64(int64_t i)
        {
            DomNode *node = dom_node_allocator.make();
            node->set_int(i);
            stack.push_back(node);
            return true;
        }

        bool Uint64(uint64_t u)
        {
            DomNode *node = dom_node_allocator.make();
            node->set_uint(u);
            stack.push_back(node);
            return true;
        }

        bool Double(double d)
        {
            DomNode *node = dom_node_allocator.make();
            node->set_double(d);
            stack.push_back(node);
            return true;
        }

        bool String(const char *str, rapidjson::SizeType length, bool copy)
        {
            DomNode *node = dom_node_allocator.make();
            node->set_string(str, length, copy);
            stack.push_back(node);
            return true;
        }

        bool StartObject()
        {
            DomNode *node = dom_node_allocator.make();
            node->set_object();
            stack.push_back(node);
            return true;
        }

        bool Key(const char *str, rapidjson::SizeType length, bool copy)
        {
            DomNode *node = dom_node_allocator.make();
            node->set_string(str, length, copy);
            stack.push_back(node);
            return true;
        }

        bool EndObject(rapidjson::SizeType memberCount)
        {
            size_t stack_len = stack.size();
            size_t obj_pos = stack_len - memberCount * 2 - 1;
            DomNode *obj = stack[obj_pos];
            for (size_t i = obj_pos + 1; i < stack_len; i += 2)
            {
                if (stack[i]->is_string() == false)
                {
                    return false;
                }
                obj->add_member(stack[i]->get_string(), stack[i + 1]);
                dom_node_allocator.delete_object(stack[i]);
            }

            stack.resize(obj_pos + 1);

            return true;
        }

        bool StartArray()
        {
            DomNode *node = dom_node_allocator.make();
            node->set_array();
            stack.push_back(node);
            return true;
        }

        bool EndArray(rapidjson::SizeType elementCount)
        {
            size_t stack_len = stack.size();
            size_t arr_pos = stack_len - elementCount - 1;
            DomNode *arr = stack[arr_pos];
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
            if (reader.HasParseError())
            {
                std::string err_msg = "json parse error: ";
                err_msg += rapidjson::GetParseError_En(reader.GetParseErrorCode());
                err_msg += " at ";
                err_msg += std::to_string(reader.GetErrorOffset());

                // free the stack before throw
                for(size_t i = 0; i < stack.size(); i++)
                {
                    dom_node_allocator.delete_object(stack[i]);
                }

                throw json_parse_error(err_msg);
            }
            return stack[0];
        }

    private:
        std::vector<DomNode *> stack;
    };
}

#endif
