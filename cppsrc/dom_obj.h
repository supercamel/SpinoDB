#ifndef DOM_OBJ_INCLUDED
#define DOM_OBJ_INCLUDED

#include <map>
#include <string>
#include "objpool.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace Spino {
    class MemberIterator 
    {
        public:
            MemberIterator(const std::map<std::string, class DomNode*> *members, std::map<std::string, class DomNode*>::const_iterator it) :
                members(members), it(it)
            {
            }

            bool operator==(const MemberIterator &other) const
            {
                return it == other.it;
            }

            bool operator!=(const MemberIterator &other)
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

            std::string get_key()
            {
                std::string result = it->first;
                return result;
            }

            const class DomView& get_value();

        private:
            const std::map<std::string, class DomNode*> *members;
            std::map<std::string, class DomNode*>::const_iterator it;

    };


    class DomObject
    {
        public:
            DomObject();
            ~DomObject();
            bool has_member(const std::string& name) const;
            void append(const std::string& member, class DomNode* val);

            const class DomView& get_member(const std::string& key, size_t len = 0) const;

            MemberIterator member_begin() const;
            MemberIterator member_end() const;

            void stringify(rapidjson::Writer<rapidjson::StringBuffer>& sb) const;

        private:
            std::map<std::string, DomNode*> members;
    };

    typedef ObjectAllocator<DomObject, 1024> DomObjectAllocatorType;
    extern DomObjectAllocatorType dom_object_allocator;

}

#endif

