#include "dom_obj.h"
#include <sstream>
#include "dom_node.h"

namespace Spino {
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

    DomObject::DomObject() {

    }

    DomObject::~DomObject() {
        for(auto& i : members) {
            //delete i.second;
            dom_node_allocator.delete_object(i.second);
        }
    }

    bool DomObject::has_member(const std::string& key) const
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

    void DomObject::append(const std::string& key, DomNode* val)
    {
        members.insert({key, val});
    }

    const DomView& DomObject::get_member(const std::string& key, size_t len) const
    {
        return *(members.find(key)->second);
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

        size_t count = 0;
        size_t sz = members.size();

        for(auto& i : members) {
            sb.Key(i.first.c_str());
            i.second->stringify(sb);
        }
        sb.EndObject();
    }

    DomObjectAllocatorType dom_object_allocator;
}


