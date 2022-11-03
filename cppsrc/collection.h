#ifndef SPINO_COLLECTION_H
#define SPINO_COLLECTION_H

#include <string>
#include <vector>
#include <memory>
#include <chrono>

#include "dom_node.h"
#include "parser.h"
#include "query_parser.h"
#include "query_executor.h"
#include "cursor.h"
#include "journal.h"
#include "rapidjson/document.h"

#include <iostream>

namespace Spino
{

    using namespace std;
    using std::chrono::duration;
    using std::chrono::duration_cast;
    using std::chrono::high_resolution_clock;
    using std::chrono::milliseconds;

    class Collection
    {
    public:
        Collection(const std::string& name, JournalWriter &jw);
        ~Collection();
        const std::string& get_name() { return name; }
        void create_index(const std::string& name);
        void drop_index(const std::string& name);
        const char* find_one(const char* query);
        DomView* find_one_dom(const std::string& query);
        DomView* find_one_dom(const std::string& index_key, const DomView& value);
        shared_ptr<Cursor> find(const std::string& query);
        size_t drop(const std::string& query, size_t limit = 1);
        void upsert(const std::string& query, DomNode *replacement);
        bool upsert(const std::string& query, const std::string& json);
        void append(DomNode *node);
        bool append(const std::string& json);
        size_t size();
        std::vector<Index>* get_indices();
        void print();

        void clear();

        void to_not_bson(std::ostream& out);

    private:
        void drop_one_by_iter(NodeListIterator id);
        rapidjson::StringBuffer sb;
        rapidjson::Writer<rapidjson::StringBuffer> writer;

        std::string name;
        ContainerType nodes;
        std::vector<Index> indices;
        uint64_t id;
        QueryExecutor executor;
        JournalWriter &jw;
    };

}

#endif
