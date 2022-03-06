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

            BaseCursor* setProjection(const char* projection);
            BaseCursor* setLimit(uint32_t max_results);

        protected: 
            void apply_projection(
                    const ValueType& proj, 
                    const ValueType& source, 
                    rapidjson::Writer<rapidjson::StringBuffer>& writer);

            DocType projection;
            bool projection_set;
            uint32_t max_results;
    };


    class LinearCursor : public BaseCursor {
        public:
            LinearCursor(ValueType& list, const char* query);
            ~LinearCursor();

            bool hasNext();
            std::string next();
            uint32_t count();

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

    class IndexCursor : public BaseCursor {
        public:
            IndexCursor(IndexIteratorRange iter_range, ValueType& collection_dom);
            ~IndexCursor();

            bool hasNext();
            std::string next();
            uint32_t count();

        private:
            ValueType& collection_dom;
            IndexIteratorRange iter_range;
            std::multimap<Spino::Value, uint32_t>::iterator iter;
            uint32_t counter = 0;
            string nextdoc;
    };

}


#endif

