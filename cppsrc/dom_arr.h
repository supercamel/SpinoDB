#ifndef DOM_ARR_INCLUDED
#define DOM_ARR_INCLUDED

#include <vector>
#include <string>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace Spino {
    class ElementIterator
    {
        public:
            ElementIterator(const std::vector<class DomNode*> *array, std::vector<class DomNode*>::const_iterator it) :
                array(array), it(it) { }

            bool operator==(const ElementIterator &other) const { return (this->it == other.it); }

            bool operator!=(const ElementIterator &other) { return it != other.it; }

            operator bool() const { return it != array->end(); }

            const class DomView& operator*();

            const class DomView* operator->();

            void operator++(int i) { it++; }

            void operator++() { ++it; }

            bool has_next();

            const DomView& next();

        private:
            const std::vector<class DomNode*> *array;
            std::vector<class DomNode*>::const_iterator it;
    };

    class DomArray
    {
        public:
            DomArray();
            ~DomArray();
            void push_back(DomNode* val);

            const class DomView& get_index(size_t pos) const;
            ElementIterator element_begin() const;
            ElementIterator element_end() const;

            void stringify(rapidjson::Writer<rapidjson::StringBuffer>& sb) const;

        private:
            std::vector<class DomNode*> elements;
    };



}

#endif


