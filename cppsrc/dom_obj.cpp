#include "dom_obj.h"
#include <sstream>
#include "dom_node.h"

namespace Spino {
    bool key_compare_less(const char* a, const char* b)
    {
        return strcmp(a, b) < 0;
    }

    const DomView& MemberIterator::operator*() const
    {
        return *(it->second);
    }

    const DomView* MemberIterator::operator->() const
    {
        return it->second;
    }

    const DomView& MemberIterator::get_value()
    {
        return *(it->second);
    }

    DomObject::DomObject() : members(key_compare_less)
    {

    }

    DomObject::~DomObject() {
        for(auto& i : members) {
            dom_node_allocator.delete_object(i.second);
            delete i.first;
        }
    }

    bool DomObject::has_member(const char* key) const
    {
        if(members.find(key) != members.end())
        {
            return true;
        }
        else 
        {
            return false;
        }
    }

    void DomObject::append(const char* key, DomNode* val)
    {
        members.insert(std::pair<const char*,DomNode*>(key,val));
    }

    void DomObject::remove(const char* key) 
    {
        auto result = members.find(key);
        if(result != members.end()) {
            dom_node_allocator.delete_object(result->second);
            delete result->first;
            members.erase(result);
        }
    }

    const DomView& DomObject::get_member(const char* key) const
    {
        return *(members.find(key)->second);
    }

    DomNode* DomObject::get_member_node(const char* key) const
    {
        return members.find(key)->second;
    }

    MemberIterator DomObject::member_begin() const
    {
        return MemberIterator(&members, members.begin());
    }

    MemberIterator DomObject::member_end() const
    {
        return MemberIterator(&members, members.end());
    }


    void DomObject::stringify(rapidjson::Writer<rapidjson::StringBuffer>& sb) const
    {
        sb.StartObject();

        for(auto& i : members) {
            sb.Key(i.first);
            i.second->stringify(sb);
        }
        sb.EndObject();
    }

    DomObjectAllocatorType dom_object_allocator;
}


