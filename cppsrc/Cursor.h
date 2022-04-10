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
    

    // typedef so you can breath while reading this
    // this is the type name of the pair that holds the start and end iterators 
    // of a range of values in an index
    typedef std::pair<
        std::multimap<Spino::Value, size_t>::iterator, 
        std::multimap<Spino::Value, size_t>::iterator
            > IndexIteratorRange;


    // eq index cursor does a binary search against an index to return results
    class Cursor {
        public:
            Cursor(shared_ptr<QueryNode> node, 
                    shared_ptr<IndexIteratorRange> iter_range, 
                    ValueType& collection_dom);
            ~Cursor();

            bool hasNext();
            std::string next();
            size_t count();
            const ValueType* nextAsJsonObj();

            std::string runScript(std::string txt);

            Cursor* setLimit(size_t limit);
            Cursor* setProjection(const char* projection);

        private:
            void apply_projection(
                    const ValueType& proj, 
                    const ValueType& source, 
                    rapidjson::Writer<rapidjson::StringBuffer>& writer);

            DocType projection;
            bool projection_set;
            size_t max_results;

            ValueType& collection_dom;
            shared_ptr<QueryNode> head;
            std::shared_ptr<IndexIteratorRange> iter_range;
            std::multimap<Spino::Value, size_t>::iterator iter;
            size_t counter = 0;
            string nextdoc;

            QueryExecutor exec;
    };

}


#endif

