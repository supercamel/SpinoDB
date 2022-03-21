//  Copyright 2022 Sam Cowen <samuel.cowen@camelsoftware.com>
//
//  Permission is hereby granted, free of charge, to any person obtaining a 
//  copy of this software and associated documentation files (the "Software"), 
//  to deal in the Software without restriction, including without limitation 
//  the rights to use, copy, modify, merge, publish, distribute, sublicense, 
//  and/or sell copies of the Software, and to permit persons to whom the 
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in 
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
//  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
//  DEALINGS IN THE SOFTWARE.


#ifndef SPINO_CURSOR_H
#define SPINO_CURSOR_H

#include <vector>
#include <list>
#include <ctime>
#include <string>
#include <map>
#include <fstream>
#include <chrono>
#include <thread>
#include <sstream>

#include "QueryNodes.h"
#include "QueryParser.h"

namespace Spino {
    class BaseCursor {
        public:
            BaseCursor();

            virtual ~BaseCursor() { };
            virtual std::string next() = 0;
            virtual bool hasNext() = 0;
            virtual uint32_t count() = 0;
            virtual const ValueType& nextAsJsonObj() = 0;

            BaseCursor* setProjection(const char* projection);
            BaseCursor* setLimit(uint32_t max_results);

            std::string runScript(std::string txt);


        protected: 
            void apply_projection(
                    const ValueType& proj, 
                    const ValueType& source, 
                    rapidjson::Writer<rapidjson::StringBuffer>& writer);

            DocType projection;
            bool projection_set;
            uint32_t max_results;
    };

    class DudCursor : public BaseCursor {
        public:
            DudCursor() { }
            ~DudCursor() { }
            std::string next() { return ""; }
            bool hasNext() { return false; }
            uint32_t count() { return 0; }
            std::string runScript(std::string txt) { return ""; }

            const ValueType& nextAsJsonObj() { return v; }
        private:
            ValueType v;
    };


    class LinearCursor : public BaseCursor {
        public:
            LinearCursor(ValueType& list, const char* query);
            ~LinearCursor();

            bool hasNext();
            std::string next();
            uint32_t count();
            const ValueType& nextAsJsonObj();

        private:
            void findNext();

            ValueType& list;
            QueryExecutor exec;
            ValueType::ConstValueIterator iter;
            std::shared_ptr<QueryNode> head;
            uint32_t limit;
            uint32_t counter = 0;
            bool has_next;
    };

    // typedef so you can breath while reading this
    // this is the type name of the pair that holds the start and end iterators 
    // of a range of values in an index
    typedef std::pair<
        std::multimap<Spino::Value, uint32_t>::iterator, 
        std::multimap<Spino::Value, uint32_t>::iterator
            > IndexIteratorRange;

    class EqIndexCursor : public BaseCursor {
        public:
            EqIndexCursor(IndexIteratorRange iter_range, ValueType& collection_dom);
            ~EqIndexCursor();

            bool hasNext();
            std::string next();
            uint32_t count();
            const ValueType& nextAsJsonObj();

        private:
            ValueType& collection_dom;
            IndexIteratorRange iter_range;
            std::multimap<Spino::Value, uint32_t>::iterator iter;
            uint32_t counter = 0;
            string nextdoc;
    };

}


#endif

