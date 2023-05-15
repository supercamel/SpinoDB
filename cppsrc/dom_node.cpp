#include "dom_node.h"
#include "dom_obj.h"
#include "dom_arr.h"
#include "parser.h"

namespace Spino
{
    DomView::DomView()
    {
        type = DOM_NODE_TYPE_NULL;
        memset(value.bytes, 0, 16);
        value.u = 0;
    }

    DomNode::DomNode()
    {
        type = DOM_NODE_TYPE_NULL;
        memset(value.bytes, 0, 16);
        value.u = 0;
    }

    DomNode::DomNode(int i)
    {
        set_int(i);
    }

    DomNode::DomNode(uint32_t u)
    {
        set_uint(u);
    }

    DomNode::DomNode(int64_t i)
    {
        type = DOM_NODE_TYPE_INT;
        value.i = i;
    }

    DomNode::DomNode(uint64_t u)
    {
        type = DOM_NODE_TYPE_UINT;
        value.u = u;
    }

    DomNode::DomNode(double d)
    {
        type = DOM_NODE_TYPE_DOUBLE;
        value.d = d;
    }

    DomNode::DomNode(const std::string &str)
    {
        set_string(str.c_str(), str.length(), true);
    }

    DomNode::DomNode(const char *str, size_t len, bool copy)
    {
        set_string(str, len, copy);
    }

    DomNode::DomNode(bool b)
    {
        type = DOM_NODE_TYPE_BOOL;
        value.b = b;
    }

    DomNode &DomNode::operator=(DomNode &other)
    {
        type = other.type;
        value = other.value;
        other.type = DOM_NODE_TYPE_INVALID;
        other.value.u = 0;
        return *this;
    }

    bool DomView::is_numeric() const
    {
        switch (type)
        {
        case DOM_NODE_TYPE_DOUBLE:
        case DOM_NODE_TYPE_INT:
        case DOM_NODE_TYPE_UINT:
        case DOM_NODE_TYPE_BOOL:
            return true;
        default:
            return false;
        }
    }

    bool DomView::is_string() const
    {
        switch (type)
        {
        case DOM_NODE_TYPE_LONG_STRING:
        case DOM_NODE_TYPE_SHORT_STRING:
            return true;
        default:
            return false;
        }
    }

    double DomView::get_numeric_as_double() const
    {
        switch (type)
        {
        case DOM_NODE_TYPE_DOUBLE:
            return value.d;
        case DOM_NODE_TYPE_INT:
            return value.i;
        case DOM_NODE_TYPE_UINT:
            return value.u;
        case DOM_NODE_TYPE_BOOL:
            return value.b;
        default:
            return 0;
        }
    }

    // assignment operator
    DomView &DomView::operator=(const DomView &other)
    {
        type = other.type;
        value = other.value;
        return *this;
    }

    bool DomView::operator==(const DomView &other) const
    {
        if ((type == DOM_NODE_TYPE_UINT) && (other.type == DOM_NODE_TYPE_UINT))
        {
            return value.u == other.value.u;
        }
        else if ((type == DOM_NODE_TYPE_INT) && (other.type == DOM_NODE_TYPE_INT))
        {
            return value.i == other.value.i;
        }
        else if (is_numeric() && other.is_numeric())
        {
            return get_numeric_as_double() == other.get_numeric_as_double();
        }
        else if (is_string() && other.is_string())
        {
            uint32_t len_a = get_string_length();
            uint32_t len_b = other.get_string_length();
            if (len_a != len_b)
            {
                return false;
            }

            for(uint32_t i = 0; i < len_a; i++)
            {
                if (get_string()[i] != other.get_string()[i])
                {
                    return false;
                }
            }

            return true;
        }
        return false;
    }

    bool DomView::operator>=(const DomView &other) const
    {
        if (is_numeric() && other.is_numeric())
        {
            return get_numeric_as_double() >= other.get_numeric_as_double();
        }
        else if (is_string() && other.is_string())
        {
            return strcmp(get_string(), other.get_string()) >= 0;
        }
        return false;
    }

    bool DomView::operator<=(const DomView &other) const
    {
        if (is_numeric() && other.is_numeric())
        {
            return get_numeric_as_double() <= other.get_numeric_as_double();
        }
        else if (is_string() && other.is_string())
        {
            return strcmp(get_string(), other.get_string()) <= 0;
        }
        return false;
    }

    bool DomView::operator>(const DomView &other) const
    {
        if (is_numeric() && other.is_numeric())
        {
            return get_numeric_as_double() > other.get_numeric_as_double();
        }
        else if (is_string() && other.is_string())
        {
            return strcmp(get_string(), other.get_string()) > 0;
        }
        return false;
    }

    bool DomView::operator<(const DomView &other) const
    {
        if (is_numeric() && other.is_numeric())
        {
            return get_numeric_as_double() < other.get_numeric_as_double();
        }
        else if (is_string() && other.is_string())
        {
            return strcmp(get_string(), other.get_string()) < 0;
        }
        return false;
    }


    bool DomView::equals(DomView* other) const
    {
        return *this == *other;
    }

    void DomNode::set_invalid()
    {
        destroy();
        type = DOM_NODE_TYPE_INVALID;
    }

    void DomNode::set_object()
    {
        destroy();
        type = DOM_NODE_TYPE_OBJECT;
        // value.ptr = (void*)(new DomObject());
        value.ptr = (void *)(dom_object_allocator.make());
    }

    DomObject *DomView::get_object() const
    {
        return (DomObject *)(value.ptr);
    }

    bool DomView::has_member(const std::string &name) const
    {
        return get_object()->has_member(name.c_str());
    }

    const DomView &DomView::get_member(const std::string &name) const
    {
        if (type == DOM_NODE_TYPE_OBJECT)
        {
            return get_object()->get_member(name.c_str());
        }
        else
        {
            throw std::runtime_error("DomView::get_member() called on non-object");
        }
    }

    const DomView &DomView::get_element(uint32_t index) const
    {
        if (type == DOM_NODE_TYPE_ARRAY)
        {
            return get_array()->get_index(index);
        }
        else
        {
            throw std::runtime_error("DomView::get_element() called on non-array");
        }
    }

    ElementIterator DomView::element_begin() const
    {
        if (type == DOM_NODE_TYPE_ARRAY)
        {
            return get_array()->element_begin();
        }
        else
        {
            throw std::runtime_error("DomView::element_begin() called on non-array");
        }
    }

    ElementIterator DomView::element_end() const
    {
        if (type == DOM_NODE_TYPE_ARRAY)
        {
            return get_array()->element_end();
        }
        else
        {
            throw std::runtime_error("DomView::element_end() called on non-array");
        }
    }

    MemberIterator DomView::member_begin() const
    {
        if (type == DOM_NODE_TYPE_OBJECT)
        {
            return get_object()->member_begin();
        }
        else
        {
            throw std::runtime_error("DomView::member_begin() called on non-object");
        }
    }

    MemberIterator DomView::member_end() const
    {
        if (type == DOM_NODE_TYPE_OBJECT)
        {
            return get_object()->member_end();
        }
        else
        {
            throw std::runtime_error("DomView::member_end() called on non-object");
        }
    }

    DomNode* DomNode::from_json(const std::string& json)
    {
        Parser parser;
        DomNode* node = parser.parse(json);
        return node;
    }

    DomNode* DomNode::from_json_file(const std::string& json) 
    {
        Parser parser;
        DomNode* node = parser.parse_file(json);
        return node;
    }

    void DomNode::set_array()
    {
        destroy();
        type = DOM_NODE_TYPE_ARRAY;
        value.ptr = (void *)(new DomArray());
    }

    DomArray *DomView::get_array() const
    {
        return (DomArray *)(value.ptr);
    }

    void DomNode::set_int(int64_t i)
    {
        destroy();
        type = DOM_NODE_TYPE_INT;
        value.i = i;
    }

    void DomNode::set_uint(uint64_t u)
    {
        destroy();
        type = DOM_NODE_TYPE_UINT;
        value.u = u;
    }

    void DomNode::set_double(double d)
    {
        destroy();
        type = DOM_NODE_TYPE_DOUBLE;
        value.d = d;
    }

    double DomView::get_double() const
    {
        return value.d;
    }

    void DomNode::set_bool(bool b)
    {
        destroy();
        type = DOM_NODE_TYPE_BOOL;
        value.b = b;
    }

    bool DomView::get_bool() const
    {
        return value.b;
    }

    void DomNode::set_string(const char *c, size_t len, bool copy)
    {
        destroy();
        if (len == 0)
        {
            len = strlen(c);
        }

        if (len <= MAX_SHORT_STRING_LEN)
        {
            type = DOM_NODE_TYPE_SHORT_STRING;
            memcpy(value.sstr.str, c, len);
            value.sstr.str[len] = 0;
            value.sstr.len = len;
        }
        else
        {
            type = DOM_NODE_TYPE_LONG_STRING;
            if (copy)
            {
                value.str.str = (char*)malloc(len + 1);
                memcpy((void*)value.str.str, c, len);
                ((char *)(value.str.str))[len] = 0;
                value.str.len = len;
            }
            else
            {
                value.str.str = c;
                value.str.len = len;
            }
        }
    }

    void DomNode::set_null()
    {
        type = DOM_NODE_TYPE_NULL;
    }

    void DomNode::copy(const DomView *other)
    {
        destroy();
        auto other_type = other->get_type();
        if (other_type == DOM_NODE_TYPE_LONG_STRING)
        {
            set_string(other->get_string(), other->value.str.len, true);
        }
        else if (other_type == DOM_NODE_TYPE_SHORT_STRING)
        {
            memcpy(value.sstr.str, other->value.sstr.str, other->value.sstr.len);
            value.sstr.len = other->value.sstr.len;
            type = DOM_NODE_TYPE_SHORT_STRING;
            
        }
        else if (other_type == DOM_NODE_TYPE_BOOL)
        {
            set_bool(other->get_bool());
        }
        else if (other_type == DOM_NODE_TYPE_DOUBLE)
        {
            set_double(other->get_double());
        }
        else if (other_type == DOM_NODE_TYPE_INT)
        {
            set_int(other->get_int());
        }
        else if (other_type == DOM_NODE_TYPE_UINT)
        {
            set_uint(other->get_uint());
        }
        else if (other_type == DOM_NODE_TYPE_ARRAY)
        {
            set_array();
            DomArray *self = get_array();
            DomArray *other_arr = other->get_array();
            for (auto it = other_arr->element_begin(); it != other_arr->element_end(); it++)
            {
                DomNode *n = dom_node_allocator.make();
                n->copy(&(*it));
                self->push_back(n);
            }
        }
        else if (other_type == DOM_NODE_TYPE_OBJECT)
        {
            set_object();
            DomObject *self = get_object();
            DomObject *other_obj = other->get_object();
            for (auto it = other_obj->member_begin(); it != other_obj->member_end(); it++)
            {
                DomNode *n = dom_node_allocator.make();
                n->copy(&(it.get_value()));
                self->append(strdup(it.get_key()), n);
            }
        }
        else if(other_type == DOM_NODE_TYPE_NULL)
        {
            set_null();
        }
        else if(other_type == DOM_NODE_TYPE_INVALID)
        {
            type = DOM_NODE_TYPE_INVALID;
            value.u = 0;
        }
    }

    void DomNode::from_not_bson(std::ifstream &fin, DOM_NODE_TYPE node_type)
    {
        switch (node_type)
        {
        case DOM_NODE_TYPE_OBJECT: // object
        {
            char typec = fin.get();
            while((typec != 0x00) && (!fin.eof()))
            {
                DomNode *child = dom_node_allocator.make();
                char* key_string = new char[8];
                uint32_t len = 0;
                uint32_t available = 8;
                char c;
                do {
                    c = fin.get();
                    key_string[len++] = c;
                    if (len >= available)
                    {
                        if(fin.eof()) {
                            break;
                        }
                        available *= 2;
                        key_string = (char*)realloc(key_string, available);
                    }
                }
                while (c != 0x00);

                if(typec == DOM_NODE_TYPE_ARRAY) {
                    child->set_array();
                }
                else if(typec == DOM_NODE_TYPE_OBJECT) {
                    child->set_object();
                }
                child->from_not_bson(fin, (DOM_NODE_TYPE)typec);
                get_object()->append(key_string, child);
                typec = fin.get(); 
            } 
        }
        break;

        case DOM_NODE_TYPE_ARRAY: // array
        {
            char typec = fin.get();
            while((typec != 0x00) && (!fin.eof()))
            {
                DomNode *child = dom_node_allocator.make();
                fin.get();

                if(typec == DOM_NODE_TYPE_ARRAY) {
                    child->set_array();
                }
                else if(typec == DOM_NODE_TYPE_OBJECT) {
                    child->set_object();
                }
                child->from_not_bson(fin, (DOM_NODE_TYPE)typec);
                get_array()->push_back(child);
                typec = fin.get();
            }
        }
        break;
        case DOM_NODE_TYPE_BOOL:
        {
            set_bool(fin.get());
        }
        break;
        case DOM_NODE_TYPE_INT:
        {
            fin.read((char *)value.bytes, 8);
            type = DOM_NODE_TYPE_INT;
        }
        break;
        case DOM_NODE_TYPE_UINT:
        {
            fin.read((char *)&value.bytes[0], 8);
            type = DOM_NODE_TYPE_UINT;
        }
        break;
        case DOM_NODE_TYPE_DOUBLE:
        {
            fin.read((char *)value.bytes, 8);
            type = DOM_NODE_TYPE_DOUBLE;
        }
        break;
        case DOM_NODE_TYPE_LONG_STRING:
        {
            fin.read((char *)&value.str.len, 4);
            char *pstr = new char[value.str.len + 1];
            fin.read(pstr, value.str.len);
            pstr[value.str.len] = 0;
            value.str.str = pstr;
            type = DOM_NODE_TYPE_LONG_STRING;
        }
        break;
        case DOM_NODE_TYPE_SHORT_STRING:
        {
            fin.read((char *)&value.sstr.len, 1);
            if(value.sstr.len > MAX_SHORT_STRING_LEN) {
                value.sstr.len = MAX_SHORT_STRING_LEN;
            }
            fin.read(value.sstr.str, value.sstr.len);
            value.sstr.str[value.sstr.len] = 0;
            type = DOM_NODE_TYPE_SHORT_STRING;
        }
        break;
        case DOM_NODE_TYPE_NULL:
        break;
        default:
            break;
        }
    }

    void DomView::to_not_bson_buff(std::vector<char> &buff) const
    {
        switch (type)
        {
        case DOM_NODE_TYPE_OBJECT: // object
        {
            for (auto it = get_object()->member_begin(); it != get_object()->member_end(); it++)
            {
                buff.push_back(it.get_value().get_type());
                //buff.insert(buff.end(), it.get_key().begin(), it.get_key().end());
                for(size_t i = 0; i < strlen(it.get_key()); i++) {
                    buff.push_back(it.get_key()[i]);
                }
                buff.push_back(0x00);
                it.get_value().to_not_bson_buff(buff);
            }
            buff.push_back(0x00);
        }
        break;

        case DOM_NODE_TYPE_ARRAY: // array
        {
            for (auto it = get_array()->element_begin(); it != get_array()->element_end(); it++)
            {
                buff.push_back(it->get_type());
                buff.push_back(0x00);
                it->to_not_bson_buff(buff);
            }
            buff.push_back(0x00);
        }
        break;
        case DOM_NODE_TYPE_BOOL:
        {
            buff.push_back(get_bool());
        }
        break;
        case DOM_NODE_TYPE_INT:
        {
            buff.insert(buff.end(), value.bytes, value.bytes + 8);
        }
        break;
        case DOM_NODE_TYPE_UINT:
        {
            buff.insert(buff.end(), value.bytes, value.bytes + 8);
        }
        break;
        case DOM_NODE_TYPE_DOUBLE:
        {
            buff.insert(buff.end(), value.bytes, value.bytes + 8);
        }
        break;
        case DOM_NODE_TYPE_LONG_STRING:
        {
            uint32_t len = value.str.len;
            buff.insert(buff.end(), (char*)&len, ((char*)&len) + 4);
            buff.insert(buff.end(), value.str.str, value.str.str + len);
        }
        break;
        case DOM_NODE_TYPE_SHORT_STRING:
        {
            buff.push_back(value.sstr.len);
            buff.insert(buff.end(), value.sstr.str, value.sstr.str + value.sstr.len);
        }
        break;
        default:
            break;
        }
    }

    void DomView::to_not_bson(std::ostream &fout) const
    {
        switch (get_type())
        {
        case DOM_NODE_TYPE_INVALID:
        case DOM_NODE_TYPE_END:
        case DOM_NODE_TYPE_NULL:
            break;
        case DOM_NODE_TYPE_BOOL:
        {
            if (get_bool())
            {
                fout.put(0x01);
            }
            else
            {
                fout.put(0x00);
            }
        }
        break;
        case DOM_NODE_TYPE_INT:
        {
            const uint8_t *pi = &value.bytes[0];
            fout.write((char *)pi, 8);
        }
        break;
        case DOM_NODE_TYPE_UINT:
        {
            const uint8_t *pi = &value.bytes[0];
            fout.write((char *)pi, 8);
        }
        break;
        case DOM_NODE_TYPE_DOUBLE:
        {
            const uint8_t *pi = &value.bytes[0];
            fout.write((char *)pi, 8);
        }
        break;
        case DOM_NODE_TYPE_LONG_STRING:
        {
            uint32_t len = value.str.len;
            fout.write((char *)&len, 4);
            fout.write(value.str.str, len);
        }
        break;
        case DOM_NODE_TYPE_SHORT_STRING:
        {
            fout.put(value.sstr.len);
            fout.write(value.sstr.str, value.sstr.len);
        }
        break;
        case DOM_NODE_TYPE_ARRAY:
        {
            const DomArray *arr = get_array();
            auto it = arr->element_begin();
            while (it != arr->element_end())
            {
                fout.put(it->get_type());
                fout.put(0x00);
                it->to_not_bson(fout);
                ++it;
            }
            fout.put(0x00);
        }
        break;
        case DOM_NODE_TYPE_OBJECT:
        {
            const DomObject *obj = get_object();
            auto it = obj->member_begin();
            while (it != obj->member_end())
            {
                fout.put(it.get_value().get_type());
                fout.write(it.get_key(), strlen(it.get_key()));
                fout.put(0x00);
                it.get_value().to_not_bson(fout);
                ++it;
            }
            fout.put(0x00);
        }
        }
    }

    void DomView::stringify(rapidjson::Writer<rapidjson::StringBuffer>& sb) const
    {
        switch (type)
        {
        case DOM_NODE_TYPE_INT:
            sb.Int64(value.i);
            break;
        case DOM_NODE_TYPE_UINT:
            sb.Uint64(value.u);
            break;
        case DOM_NODE_TYPE_DOUBLE:
            sb.Double(value.d);
            break;
        case DOM_NODE_TYPE_BOOL:
            sb.Bool(value.b);
            break;
        case DOM_NODE_TYPE_NULL:
            sb.Null();
            break;
        case DOM_NODE_TYPE_LONG_STRING:
        case DOM_NODE_TYPE_SHORT_STRING:
            sb.String(get_string(), get_string_length());
            break;
        case DOM_NODE_TYPE_OBJECT:
            get_object()->stringify(sb);
            break;
        case DOM_NODE_TYPE_ARRAY:
            get_array()->stringify(sb);
            break;
        default:
            break;
        }
    }

    std::string DomView::stringify() const 
    {
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
        stringify(writer);
        return sb.GetString();
    }

    DomNode::~DomNode()
    {
        destroy();
    }

    void DomNode::destroy()
    {
        switch (type)
        {
        case DOM_NODE_TYPE_INT:
        case DOM_NODE_TYPE_UINT:
        case DOM_NODE_TYPE_DOUBLE:
        case DOM_NODE_TYPE_BOOL:
        case DOM_NODE_TYPE_NULL:
        case DOM_NODE_TYPE_INVALID:
        case DOM_NODE_TYPE_SHORT_STRING:
        case DOM_NODE_TYPE_END:
            break;
        case DOM_NODE_TYPE_LONG_STRING:
        {
            delete value.str.str;
        }
        break;
        case DOM_NODE_TYPE_ARRAY:
        {
            delete get_array();
        }
        break;
        case DOM_NODE_TYPE_OBJECT:
        {
            // delete get_object();
            dom_object_allocator.delete_object(get_object());
        }
        break;
        }
        type = DOM_NODE_TYPE_NULL;
    }

    void DomNode::add_member(const std::string &name, DomNode *node)
    {
        if (type == DOM_NODE_TYPE_OBJECT)
        {
            get_object()->append(strdup(name.c_str()), node);
        }
        else
        {
            cout << "add_member: not an object" << endl;
        }
    }

    void DomNode::remove_member(const std::string &name)
    {
        if (type == DOM_NODE_TYPE_OBJECT)
        {
            get_object()->remove(name.c_str());
        }
        else
        {
            cout << "remove_member: not an object" << endl;
        }
    }

    void DomNode::push_back(DomNode *node)
    {
        if (type == DOM_NODE_TYPE_ARRAY)
        {
            get_array()->push_back(node);
        }
        else
        {
            cout << "push_back: not an array" << endl;
        }
    }

    void DomNode::pop_back()
    {
        if (type == DOM_NODE_TYPE_ARRAY)
        {
            get_array()->pop_back();
        }
        else
        {
            cout << "pop_back: not an array" << endl;
        }
    }

    DomNode* DomNode::get_element_node(uint32_t index)
    {
        if (type == DOM_NODE_TYPE_ARRAY)
        {
            return get_array()->get_index_node(index);
        }
        else
        {
            cout << "get_element: not an array" << endl;
            return nullptr;
        }
    }

    DomNode* DomNode::get_member_node(const std::string &name)
    {
        if (type == DOM_NODE_TYPE_OBJECT)
        {
            return get_object()->get_member_node(name.c_str());
        }
        else
        {
            cout << "get_member: not an object" << endl;
            return nullptr;
        }
    }

    ElementIterator DomNode::erase(ElementIterator iter) {
        if (type == DOM_NODE_TYPE_ARRAY)
        {
            return get_array()->erase(iter);
        }
        else
        {
            cout << "erase: not an array" << endl;
        }
        return iter;
    }

    DomNodeAllocatorType dom_node_allocator;

}
