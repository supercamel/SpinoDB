#ifndef DOM_OBJ_INCLUDED
#define DOM_OBJ_INCLUDED

#include <map>
#include "objpool.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace Spino {
    typedef std::map<const char*, class DomNode*, bool(*)(const char*, const char*)> DomMapType;
    class MemberIterator 
    {
        public:
            MemberIterator(const DomMapType *members, DomMapType::const_iterator it) :
                members(members), it(it)
            {
            }

            bool operator==(const MemberIterator &other) const
            {
                return it == other.it;
            }

            bool operator!=(const MemberIterator &other) const
            {
                return it != other.it;
            }

            const class DomView& operator*() const;
            const class DomView* operator->() const;

            void operator++(int i)
            {
                ++it;
            }

            void operator++()
            {
                ++it;
            }

            bool has_next()
            {
                return it != members->end();
            }

            const char* get_key()
            {
                const char* result = it->first;
                return result;
            }

            const class DomView& get_value();

        private:
            const DomMapType *members;
            DomMapType::const_iterator it;

    };


    class DomObject
    {
        public:
            DomObject();
            ~DomObject();
            bool has_member(const char* name) const;
            void append(const char* member, class DomNode* val);
            void remove(const char* member);

            const class DomView& get_member(const char* key) const;
            DomNode* get_member_node(const char* key) const;

            MemberIterator member_begin() const;
            MemberIterator member_end() const;

            void stringify(rapidjson::Writer<rapidjson::StringBuffer>& sb) const;

        private:
            DomMapType members;
    };

    typedef ObjectAllocator<DomObject, 1024> DomObjectAllocatorType;
    extern DomObjectAllocatorType dom_object_allocator;

}

#endif

