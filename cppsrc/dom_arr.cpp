#include "dom_arr.h"
#include "dom_node.h"
#include <sstream>

namespace Spino {

    const DomView& ElementIterator::operator*() { DomView* n = *it; return *n; }

    const class DomView* ElementIterator::operator->() { return (*it); }

    bool ElementIterator::has_next() { return it != array->end(); }

    const DomView& ElementIterator::get_value() { return **it; }
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

    DomNode* DomArray::get_index_node(size_t pos) const
    {
        return elements.at(pos);
    }

    void DomArray::push_back(DomNode* val)
    {
        elements.push_back(val);
    }

    void DomArray::pop_back()
    {
        if(elements.size() > 0) {
            auto& last_node = elements.back();
            dom_node_allocator.delete_object(last_node);
            elements.pop_back();
        }
    }

    ElementIterator DomArray::erase(ElementIterator iter)
    {
        DomNode* node = *(iter.it);
        dom_node_allocator.delete_object(node);
        iter.it = elements.erase(iter.it);
        return iter;
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

        for(auto& element : elements) {
            element->stringify(sb);
        }
        sb.EndArray();
    }

}
