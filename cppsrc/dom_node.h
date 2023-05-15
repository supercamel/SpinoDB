#ifndef SPINO_DOM_NODE_H
#define SPINO_DOM_NODE_H

#include <iostream>
using namespace std;


#include <cstdint>
#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>
#include <map>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/rapidjson.h"
#include "dom_arr.h"
#include "dom_obj.h"
#include "objpool.h"

namespace Spino
{

    inline std::string escape(const std::string &str)
    {
        std::string result;
        for (char c : str)
        {
            if (c == '"')
            {
                result += "\\\"";
            }
            else if (c == '\\')
            {
                result += "\\\\";
            }
            else if (c == '\n')
            {
                result += "\\n";
            }
            else if (c == '\t')
            {
                result += "\\t";
            }
            else
            {
                result += c;
            }
        }
        return result;
    }

    inline std::string unescape(const std::string &str)
    {
        std::string result;
        unsigned int len = str.length();
        unsigned int i = 0;
        while (i < len)
        {
            char c = str[i++];
            if (c == '\\')
            {
                c = str[i++];
                if (c == 'n')
                {
                    result += '\n';
                }
                else if (c == '\\')
                {
                    result += '\\';
                }
                else if (c == 't')
                {
                    result += '\t';
                }
                else if (c == '"')
                {
                    result += '"';
                }
                else
                {
                    cout << "SpinoDB:: unescape: unknown escape sequence in string.\n"
                        << str << endl;
                    cout << str[i - 1] << endl;
                    throw std::runtime_error("Invalid escape sequence");
                }
            }
            else
            {
                result += c;
            }
        }
        return result;
    }

    enum DOM_NODE_TYPE : uint8_t
    {
        DOM_NODE_TYPE_INVALID,
        DOM_NODE_TYPE_INT,
        DOM_NODE_TYPE_UINT,
        DOM_NODE_TYPE_DOUBLE,
        DOM_NODE_TYPE_LONG_STRING,
        DOM_NODE_TYPE_SHORT_STRING,
        DOM_NODE_TYPE_BOOL,
        DOM_NODE_TYPE_OBJECT,
        DOM_NODE_TYPE_ARRAY,
        DOM_NODE_TYPE_NULL,
        DOM_NODE_TYPE_END
    };

    inline const char *dom_node_type_to_string(DOM_NODE_TYPE type)
    {
        switch (type)
        {
            case DOM_NODE_TYPE_INVALID:
                return "invalid";
            case DOM_NODE_TYPE_INT:
                return "int";
            case DOM_NODE_TYPE_UINT:
                return "uint";
            case DOM_NODE_TYPE_DOUBLE:
                return "double";
            case DOM_NODE_TYPE_LONG_STRING:
            case DOM_NODE_TYPE_SHORT_STRING:
                return "string";
            case DOM_NODE_TYPE_BOOL:
                return "bool";
            case DOM_NODE_TYPE_OBJECT:
                return "object";
            case DOM_NODE_TYPE_ARRAY:
                return "array";
            case DOM_NODE_TYPE_NULL:
                return "null";
            case DOM_NODE_TYPE_END:
                return "";
        }
        return "";
    }

    struct String {
        uint32_t len;
        const char *str;
    };

    struct ShortString {
        uint8_t len;
        char str[15];
    };

    union DomNodeValue
    {
        int64_t i;
        uint64_t u;
        double d;
        String str;
        ShortString sstr;
        bool b;
        void* ptr;
        uint8_t bytes[16];
    };


    class DomView
    {
        public:
            DomView();

            constexpr DomView(const DomView &other) : type(other.type), value(other.value) {}

            friend class DomNode;

            bool is_numeric() const;
            double get_numeric_as_double() const;

            bool is_string() const;

            // assignment operator
            DomView& operator=(const DomView& other);


            bool operator==(const DomView &other) const;
            bool operator>=(const DomView &other) const;
            bool operator<=(const DomView &other) const;
            bool operator>(const DomView &other) const;
            bool operator<(const DomView &other) const;

            bool equals(DomView *other) const;


            inline int64_t get_int() const { return value.i; }
            inline uint64_t get_uint() const { return value.u; }
            const char* get_string() const {
                switch(type) {
                    case DOM_NODE_TYPE_LONG_STRING:
                        return value.str.str;
                    case DOM_NODE_TYPE_SHORT_STRING:
                        return value.sstr.str;
                    default:
                        return nullptr;
                }
            }

            uint32_t get_string_length() const {
                switch(type) {
                    case DOM_NODE_TYPE_LONG_STRING:
                        return value.str.len;
                    case DOM_NODE_TYPE_SHORT_STRING:
                        return value.sstr.len;
                    default:
                        return 0;
                }
            }

            double get_double() const;
            bool get_bool() const;
            inline DOM_NODE_TYPE get_type() const { return type; }

            void to_not_bson(std::ostream &fout) const;
            void to_not_bson_buff(std::vector<char> &buff) const;

            bool has_member(const std::string& name) const;
            const DomView& get_member(const std::string& name) const;
            const DomView& get_element(uint32_t index) const;

            ElementIterator element_begin() const;
            ElementIterator element_end() const;

            MemberIterator member_begin() const;
            MemberIterator member_end() const;

            void stringify(rapidjson::Writer<rapidjson::StringBuffer>& sb) const;
            std::string stringify() const;
        protected:
            DOM_NODE_TYPE type;
            DomNodeValue value;

            class DomObject* get_object() const;
            class DomArray* get_array() const;
    };

    class DomNode : public DomView
    {
        public:
            friend class DomObject;
            friend class DomArray;

            DomNode();
            DomNode(DOM_NODE_TYPE _type);
            DomNode(const std::string& str);
            DomNode(const char *str, size_t len, bool copy);
            DomNode(int i);
            DomNode(uint32_t u);
            DomNode(int64_t i);
            DomNode(uint64_t u);
            DomNode(double d);
            DomNode(bool b);
            DomNode &operator=(DomNode &other);

            static DomNode* from_json(const std::string& json);
            static DomNode* from_json_file(const std::string& filename);

            DomNode(const DomNode& other) = delete;
            ~DomNode();

            void set_invalid();
            void set_object();
            void set_array();
            void set_int(int64_t i);
            void set_uint(uint64_t u);
            void set_double(double d);
            void set_bool(bool b);
            void set_string(const char* c, size_t len = 0, bool copy = true);
            void set_null();

            void copy(const DomView *other);
            void from_not_bson(std::ifstream &fin, DOM_NODE_TYPE node_type);

            void add_member(const std::string& name, DomNode* node);
            void remove_member(const std::string& name);
            void push_back(DomNode* node);
            void pop_back();
            ElementIterator erase(ElementIterator iter);

            DomNode* get_element_node(uint32_t index);
            DomNode* get_member_node(const std::string& name);

            void destroy();
        private:

            static constexpr uint32_t MAX_SHORT_STRING_LEN = 14;
    };

    typedef ObjectAllocator<DomNode, 1024> DomNodeAllocatorType;
    extern DomNodeAllocatorType dom_node_allocator;
}

#endif
