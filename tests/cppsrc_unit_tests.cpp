#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>

#include "collection.h"
#include "database.h"
#include "parser.h"
#include "query_parser.h"

using namespace Spino;

namespace
{
    std::string temp_path(const std::string &name)
    {
        std::ostringstream ss;
        ss << "/tmp/spinodb_" << name << "_" << getpid();
        return ss.str();
    }

    std::string read_file(const std::string &path)
    {
        std::ifstream in(path);
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    void truncate_file(const std::string &path)
    {
        std::ofstream out(path, std::ofstream::out | std::ofstream::trunc);
    }

    void test_cursor_no_match_does_not_deref_end()
    {
        JournalWriter jw;
        Collection col("people", jw);
        col.append("{\"name\":\"Ada\"}");

        auto cursor = col.find("{name:\"Grace\"}");
        assert(cursor->has_next() == false);
        assert(cursor->next_dom() == nullptr);
    }

    void test_update_refreshes_index_entries()
    {
        JournalWriter jw;
        Collection col("people", jw);
        col.create_index("name");
        col.append("{\"name\":\"Ada\",\"age\":36}");

        DomNode *patch = DomNode::from_json("{\"name\":\"Grace\"}");
        col.update("{name:\"Ada\"}", patch);
        dom_node_allocator.delete_object(patch);

        assert(col.find_one("{name:\"Ada\"}").empty());
        DomView *updated = col.find_one_dom("{name:\"Grace\"}");
        assert(updated != nullptr);
        assert(std::string(updated->get_member("name").get_string()) == "Grace");
    }

    void test_successful_update_is_journaled()
    {
        const std::string path = temp_path("update_journal");
        std::remove(path.c_str());

        JournalWriter jw;
        jw.set_path(path);
        jw.set_enabled(true);
        Collection col("people", jw);
        col.create_index("name");
        col.append("{\"name\":\"Ada\"}");
        truncate_file(path);

        DomNode *patch = DomNode::from_json("{\"name\":\"Grace\"}");
        col.update("{name:\"Ada\"}", patch);
        dom_node_allocator.delete_object(patch);

        const std::string journal = read_file(path);
        assert(journal.find("\"cmd\":\"update\"") != std::string::npos);
        assert(journal.find("\"collection\":\"people\"") != std::string::npos);
        std::remove(path.c_str());
    }

    void test_drop_collection_deletes_and_journals()
    {
        const std::string path = temp_path("drop_collection_journal");
        std::remove(path.c_str());

        Database db;
        db.enable_journal(path);
        db.get_collection("scratch")->append("{\"v\":1}");
        truncate_file(path);

        db.drop_collection("scratch");
        assert(db.has_collection("scratch") == false);

        const std::string journal = read_file(path);
        assert(journal.find("\"cmd\":\"dropCollection\"") != std::string::npos);
        assert(journal.find("\"collection\":\"scratch\"") != std::string::npos);
        std::remove(path.c_str());
    }

    void test_parse_file_missing_throws()
    {
        bool threw = false;
        try
        {
            DomNode::from_json_file(temp_path("missing_json"));
        }
        catch (const json_parse_error &)
        {
            threw = true;
        }
        assert(threw);
    }

    void test_query_operator_prefixes_are_rejected()
    {
        JournalWriter jw;
        Collection col("people", jw);
        IndexIteratorRange range;
        bool threw = false;

        try
        {
            QueryParser parser(*col.get_indices(), "{name:{$integer:[\"Ada\"]}}");
            parser.parse_query(range);
        }
        catch (const parse_error &)
        {
            threw = true;
        }
        assert(threw);
    }

    void test_identifier_at_end_does_not_read_past_end()
    {
        JournalWriter jw;
        Collection col("people", jw);
        IndexIteratorRange range;
        bool threw = false;

        try
        {
            QueryParser parser(*col.get_indices(), "{name");
            parser.parse_query(range);
        }
        catch (const parse_error &)
        {
            threw = true;
        }
        assert(threw);
    }
}

int main()
{
    test_cursor_no_match_does_not_deref_end();
    test_update_refreshes_index_entries();
    test_successful_update_is_journaled();
    test_drop_collection_deletes_and_journals();
    test_parse_file_missing_throws();
    test_query_operator_prefixes_are_rejected();
    test_identifier_at_end_does_not_read_past_end();

    std::cout << "cppsrc unit tests passed\n";
    return 0;
}
