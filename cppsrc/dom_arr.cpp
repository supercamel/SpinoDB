#include "dom_arr.h"
#include "dom_node.h"
#include <sstream>

namespace Spino {

    const DomView& ElementIterator::operator*() { DomView* n = *it; return *n; }

    const class DomView* ElementIterator::operator->() { return (*it); }

    bool ElementIterator::has_next() { return it != array->end(); }

    const DomView& ElementIterator::next() { return **it++; }


    DomArray::DomArray() {

    }

    DomArray::~DomArray() {
        for(auto i : elements) {
            dom_node_allocator.delete_object(i);
        }
    }

    const DomView& DomArray::get_index(size_t pos) const
    {
        return *elements.at(pos);
    }

    void DomArray::push_back(DomNode* val)
    {
        elements.push_back(val);
    }


    ElementIterator DomArray::element_begin() const
    {
        return ElementIterator(&elements, elements.begin());
    }

    ElementIterator DomArray::element_end() const
    {
        return ElementIterator(&elements, elements.end());
    }

    void DomArray::stringify(rapidjson::Writer<rapidjson::StringBuffer>& sb) const
    {
        sb.StartArray();
        size_t count = 0;
        size_t size = elements.size();

        for(auto& element : elements) {
            element->stringify(sb);
        }
        sb.EndArray();
    }

}
