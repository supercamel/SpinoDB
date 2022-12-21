#include "database.h"
#include <memory>
#include <sstream>

namespace Spino
{
    Database::Database()
    {
        kvstore = new Collection("__SpinoKeyValueStore__", jw);
        kvstore->create_index("k");
    }

    Database::~Database() {
        for(auto& col : collections) {
            delete col;
        }
        delete kvstore;
    }

    Collection* Database::get_collection(const std::string &name)
    {
        if (std::string(name) == std::string("__SpinoKeyValueStore__"))
        {
            return kvstore;
        }

        for (auto col : collections)
        {
            if (std::string(col->get_name()) == name)
            {
                return col;
            }
        }

        collections.push_back(new Collection(name, jw));
        return collections.back();
    }

    bool Database::has_collection(const std::string &name) const
    {
        for (auto col : collections)
        {
            if (col->get_name() == name)
            {
                return true;
            }
        }

        return false;
    }

    vector <string> Database::listCollections() const {
        vector <string> collectionNames;
        for (auto collection: collections) {
            collectionNames.push_back(collection->get_name());
        }
        return collectionNames;
    }


    void Database::drop_collection(const std::string &name) {
        for (auto iter = collections.begin(); iter != collections.end(); iter++) {
            if (std::string((*iter)->get_name()) == name) {
                collections.erase(iter);
                delete *iter;
                return;
            }
        }

        if (jw.get_enabled()) {
            rapidjson::StringBuffer sb;
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            writer.StartObject();
            writer.Key("cmd");
            writer.String("dropCollection");
            writer.Key("collection");
            writer.String(name.c_str());

            writer.EndObject();
            jw.append(sb.GetString());
        }
    }

    void Database::set_bool_value(const std::string &key, bool value)
    {
        DomNode *doc = dom_node_allocator.make();
        doc->set_object();

        DomNode *val = dom_node_allocator.make();
        val->set_bool(value);

        DomNode* k = dom_node_allocator.make();
        k->set_string(key.c_str(), key.length(), true);

        doc->add_member("v", val);
        doc->add_member("k", k);

        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        kvstore->drop(query, UINT32_MAX);
        kvstore->append(doc);
    }

    bool Database::get_bool_value(const std::string &key) const
    {
        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        DomView *result = kvstore->find_one_dom(query.c_str());
        if (result)
        {
            return result->get_member("v").get_bool();
        }
        return false;
    }

    void Database::set_int_value(const std::string &key, int value)
    {
        DomNode *doc = dom_node_allocator.make();
        doc->set_object();

        DomNode *val = dom_node_allocator.make();
        val->set_int(value);
        doc->add_member("v", val);

        DomNode* k = dom_node_allocator.make();
        k->set_string(key.c_str(), key.length(), true);
        doc->add_member("k", k);

        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        kvstore->drop(query.c_str(), UINT32_MAX);
        kvstore->append(doc);
    }

    int Database::get_int_value(const std::string &key) const
    {
        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        DomView *result = kvstore->find_one_dom(query);
        if (result)
        {
            return result->get_member("v").get_int();
        }
        return -1;
    }

    void Database::set_uint_value(const std::string &key, unsigned int value)
    {
        DomNode *doc = dom_node_allocator.make();
        doc->set_object();

        DomNode *val = dom_node_allocator.make();
        val->set_uint(value);
        doc->add_member("v", val);

        DomNode* k = dom_node_allocator.make();
        k->set_string(key.c_str(), key.length(), true);
        doc->add_member("k", k);

        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        kvstore->drop(query, UINT32_MAX);
        kvstore->append(doc);
    }

    unsigned int Database::get_uint_value(const std::string &key) const
    {
        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        DomView *result = kvstore->find_one_dom(query);
        if (result)
        {
            return result->get_member("v").get_uint();
        }
        return -1;
    }

    void Database::set_double_value(const std::string &key, double value)
    {
        DomNode *doc = dom_node_allocator.make();
        doc->set_object();

        DomNode *val = dom_node_allocator.make();
        val->set_double(value);
        doc->add_member("v", val);

        DomNode* k = dom_node_allocator.make();
        k->set_string(key.c_str(), key.length(), true);
        doc->add_member("k", k);

        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        kvstore->drop(query, UINT32_MAX);
        kvstore->append(doc);
    }

    double Database::get_double_value(const std::string &key) const
    {
        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        DomView *result = kvstore->find_one_dom(query);
        if (result)
        {
            return result->get_member("v").get_double();
        }
        return 0.0;
    }

    void Database::set_string_value(const std::string &key, const std::string &value)
    {
        DomNode *doc = dom_node_allocator.make();
        doc->set_object();

        DomNode *val = dom_node_allocator.make();
        val->set_string(value.c_str(), value.length(), true);
        doc->add_member("v", val);

        DomNode* k = dom_node_allocator.make();
        k->set_string(key.c_str(), key.length(), true);
        doc->add_member("k", k);


        std::string query = "{k:\"";
        query += key;
        query += "\"}";

        kvstore->drop(query, UINT32_MAX);
        kvstore->append(doc);
    }

    const char* Database::get_string_value(const std::string &key) const
    {
        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        const DomView *result = kvstore->find_one_dom(query);
        if (result)
        {
            return result->get_member("v").get_string();
        }
        
        return "";
    }

    bool Database::has_key(const std::string &key) const
    {
        std::string query = "{k:\"";
        query += key;
        query += "\"}";
        const DomView *result = kvstore->find_one_dom(query);
        if (result)
        {
            return true;
        }
        return false;
    }

    FILE_ERROR Database::save(const std::string &path) const
    {
        std::string tmppath = path;
        ofstream out(tmppath, ios::out | ios::binary);
        if (!out)
        {
            return FILE_ERROR_WRITE_ERROR;
        }

        out.put(DOM_NODE_TYPE_ARRAY);
        const std::string &keystore_name = kvstore->get_name();
        out.write(keystore_name.c_str(), keystore_name.length() + 1);
        kvstore->to_not_bson(out);
        out.put(0x00);

        for (auto &col : collections)
        {
            out.put(DOM_NODE_TYPE_ARRAY);
            const std::string col_name = col->get_name();
            out.write(col_name.c_str(), col_name.length() + 1);
            col->to_not_bson(out);
            out.put(0x00);
        }
        out.put(0x00);
        out.flush();
        out.close();

        // clear the journal
        if (jw.get_enabled())
        {
            std::ofstream ofs;
            ofs.open(jw.get_path(), std::ofstream::out | std::ofstream::trunc);
            ofs.close();
        }

        return FILE_ERROR_NONE;
    }

    FILE_ERROR Database::load(const std::string &path)
    {
        ifstream ifile(path, ios::in | ios::binary);
        if (!ifile)
        {
            return FILE_ERROR_PARSE_ERROR;
        }

        delete kvstore;
        kvstore = new Collection("__SpinoKeyValueStore__", jw);
        kvstore->create_index("k");

        for(auto &col : collections)
        {
            delete col;
        }
        collections.erase(collections.begin(), collections.end());


        while (ifile.get() == DOM_NODE_TYPE_ARRAY)
        {
            std::string col_name;
            while (ifile.peek() != 0x00)
            {
                col_name += ifile.get();
            }
            ifile.get();

            if (col_name == std::string("__SpinoKeyValueStore__"))
            {
                while (ifile.get() == DOM_NODE_TYPE_OBJECT)
                {
                    DomNode *doc = dom_node_allocator.make();
                    doc->set_object();
                    doc->from_not_bson(ifile, DOM_NODE_TYPE_OBJECT);
                    kvstore->append(doc);
                }
            }
            else
            {
                auto col = get_collection(col_name.c_str());
                while (ifile.get() == DOM_NODE_TYPE_OBJECT)
                {
                    DomNode *doc = dom_node_allocator.make();
                    doc->set_object();
                    doc->from_not_bson(ifile, DOM_NODE_TYPE_OBJECT);
                    col->append(doc);
                }
            }
        }
        return FILE_ERROR_NONE;
    }

    void Database::save_not_bson_collection(const std::string &col_name, const std::string &path) const
    {
        for (auto &col : collections)
        {
            if (col_name == col->get_name())
            {
                ofstream out(path, ios::out | ios::binary);
                auto cursor = col->find("{}");

                while (cursor->has_next())
                {
                    out.put(DOM_NODE_TYPE_OBJECT);
                    cursor->next_dom()->to_not_bson(out);
                }
                out.put(0x00);
            }
        }
    }

    void Database::save_not_bson_keystore(const std::string &path) const
    {
        ofstream out(path, ios::out | ios::binary);

        auto cursor = kvstore->find("{}");
        while (cursor->has_next())
        {
            out.put(DOM_NODE_TYPE_OBJECT);
            cursor->next_dom()->to_not_bson(out);
        }
        out.put(0x00);
    }

    void Database::load_not_bson_collection(const std::string &col_name, const std::string &path)
    {
        auto col = get_collection(col_name);

        ifstream ifile(path, ios::in | ios::binary);

        while (ifile.get() == DOM_NODE_TYPE_OBJECT)
        {
            DomNode *doc = dom_node_allocator.make();
            doc->set_object();
            doc->from_not_bson(ifile, DOM_NODE_TYPE_OBJECT);
            col->append(doc);
        }
    }

    void Database::save_json(const std::string &path) const
    {
        ofstream ofile;
        ofile.open(path);
        ofile << "{";

        save_json_collection(ofile, kvstore);

        if (collections.size())
        {
            ofile << ",";
            auto iter = collections.begin();
            while (iter != collections.end())
            {
                save_json_collection(ofile, *iter);
                iter++;
                if (iter != collections.end())
                {
                    ofile << ",";
                }
            }
        }

        ofile << "}";
        ofile.close();
    }

    void Database::enable_journal(const std::string &jpath)
    {
        jw.set_enabled(true);
        jw.set_path(jpath);
    }

    void Database::disable_journal()
    {
        jw.set_enabled(false);
    }

    void Database::consolidate(const std::string &path)
    {
        bool priorState = jw.get_enabled();
        jw.set_enabled(false);

        // read the journal file and execute the commands
        std::ifstream file(jw.get_path());
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                std::string result = execute(line);
                if(result != "{\"msg\":\"OK\"}") {
                    cout << result << endl;
                }
            }
            file.close();
        }

        jw.set_enabled(priorState);

        // write the db to disk
        // noting that save will also erase the journal
        save(path);
    }

    std::string Database::execute(const std::string &json)
    {
        Parser parser;
        DomNode *doc = parser.parse(json);
        std::string result;

        result = "{\"error\":\"unknown command\"}";
        if (doc == nullptr)
        {
            return "Error: Invalid JSON";
        }

        if (!doc->has_member("cmd"))
        {
            result = "{\"error\":\"Missing cmd field\"}";
            dom_node_allocator.delete_object(doc);
            return result;
        }

        if (!doc->get_member("cmd").is_string())
        {
            result = "{\"error\":\"cmd field must be of type string\"}";
            dom_node_allocator.delete_object(doc);
            return result;
        }

        std::string cmd = doc->get_member("cmd").get_string();
        if (cmd == "count")
        {
            if (doc->has_member("collection"))
            {
                std::stringstream ss;
                ss << "{\"r\":1,\"count\":" << collections.size() << "}";
                result = ss.str();
            }
            else
            {
                auto col = get_collection(doc->get_member("collection").get_string());
                std::stringstream ss;
                ss << "{\"r\":1,\"count\":" << col->size() << "}";

                result = ss.str();
            }
        }

        if (cmd == "dropCollection")
        {
            if (doc->has_member("collection"))
            {
                drop_collection(doc->get_member("collection").get_string());
                result = "{\"msg\":\"OK\"}";
            }
            else
            {
                result = "{\"error\":\"dropCollection command is missing a collection name\"}";
            }
        }

        if (cmd == "createIndex")
        {
            if (doc->has_member("collection") && (doc->has_member("field")))
            {
                DomView colnode = doc->get_member("collection");
                DomView fieldnode = doc->get_member("field");
                if ((!colnode.is_string()) || (!fieldnode.is_string()))
                {
                    result = "{\"error\":\"both collection and field must be of type string\"}";
                    goto cleanup;
                }

                auto col = get_collection(colnode.get_string());
                col->create_index(fieldnode.get_string());
                result = "{\"msg\":\"OK\"}";
            }
            else
            {
                result = "{\"error\":\"Missing collection or 'field' field in createIndex command\"}";
            }
        }

        if (cmd == "dropIndex")
        {
            if (doc->has_member("collection") && (doc->has_member("field")))
            {
                DomView colnode = doc->get_member("collection");
                DomView fieldnode = doc->get_member("field");
                if ((!colnode.is_string()) || (!fieldnode.is_string()))
                {
                    result = "{\"error\":\"both collection and field must be of type string\"}";
                    goto cleanup;
                }

                auto col = get_collection(colnode.get_string());
                col->drop_index(fieldnode.get_string());
                result = "{\"msg\":\"OK\"}";
            }
            else
            {
                result = "{\"error\":\"Missing collection or 'field' field in createIndex command\"}";
            }
        }

        if (cmd == "findOne")
        {
            if (doc->has_member("collection") && doc->has_member("query"))
            {
                DomView colnode = doc->get_member("collection");
                DomView querynode = doc->get_member("query");
                if ((!colnode.is_string()) || (!querynode.is_string()))
                {
                    result = "{\"error\":\"both collection and query must be of type string\"}";
                    goto cleanup;
                }

                auto col = get_collection(colnode.get_string());
                return col->find_one(querynode.get_string());
            }
            else
            {
                result = "{\"error\":\"Collection and query fields are required\"}";
            }
        }

        if (cmd == "find")
        {
            if (doc->has_member("collection") && doc->has_member("query"))
            {
                DomView colnode = doc->get_member("collection");
                DomView querynode = doc->get_member("query");
                if ((!colnode.is_string()) || (!querynode.is_string()))
                {
                    result = "{\"error\":\"both collection and query must be of type string\"}";
                    goto cleanup;
                }

                size_t limit = 20;
                if (doc->has_member("limit"))
                {
                    DomView limitnode = doc->get_member("limit");
                    if (limitnode.get_type() == DOM_NODE_TYPE_UINT)
                    {
                        limit = limitnode.get_uint();
                    }
                }

                auto col = get_collection(colnode.get_string());
                auto cursor = col->find(querynode.get_string());
                cursor->set_limit(limit);
                string r = "[";
                while (cursor->has_next())
                {
                    r += cursor->next();
                    if (cursor->has_next())
                    {
                        r += ",";
                    }
                }
                r += "]";
                result = r;
            }
            else
            {
                result = "{\"error\":\"Collection and query fields are required\"}";
            }
        }

        if (cmd == "append")
        {
            if (doc->has_member("collection") && doc->has_member("document"))
            {
                DomView colnode = doc->get_member("collection");
                DomView docnode = doc->get_member("document");
                if ((!colnode.is_string()) || (!docnode.is_string()))
                {
                    result = "{\"error\":\"both collection and document must be of type string\"}";
                    goto cleanup;
                }

                auto col = get_collection(colnode.get_string());
                if (col->append(docnode.get_string()))
                {
                    result = "{\"msg\":\"OK\"}";
                }
                else
                {
                    result = "{\"error\":\"parse error\"}";
                }
            }
            else
            {
                result = "{\"error\":\"Collection and document fields are required\"}";
            }
        }

        if(cmd == "update") 
        {
            if (doc->has_member("collection") && doc->has_member("query") && doc->has_member("document"))
            {
                DomView colnode = doc->get_member("collection");
                DomView querynode = doc->get_member("query");
                DomView docnode = doc->get_member("document");
                if ((!colnode.is_string()) || (!querynode.is_string()) || (!docnode.is_string()))
                {
                    result = "{\"error\":\"both collection and document must be of type string\"}";
                    goto cleanup;
                }

                auto col = get_collection(colnode.get_string());
                col->update(querynode.get_string(), docnode.get_string());
                result = "{\"msg\":\"OK\"}";
            }
            else
            {
                result = "{\"error\":\"Collection, query and document fields are required\"}";
            }
        }

        if (cmd == "upsert")
        {
            if (doc->has_member("collection") && doc->has_member("query") && doc->has_member("document"))
            {
                DomView colnode = doc->get_member("collection");
                DomView querynode = doc->get_member("query");
                DomView docnode = doc->get_member("document");
                if ((!colnode.is_string()) || (!querynode.is_string()) || (!docnode.is_string()))
                {
                    result = "{\"error\":\"both collection and document must be of type string\"}";
                    goto cleanup;
                }

                auto col = get_collection(colnode.get_string());
                col->upsert(querynode.get_string(), docnode.get_string());
                result = "{\"msg\":\"OK\"}";
            }
            else
            {
                result = "{\"error\":\"Collection, query and document fields are required\"}";
            }
        }

        if (cmd == "drop")
        {
            if (doc->has_member("collection") && doc->has_member("query") && doc->has_member("limit"))
            {
                DomView colnode = doc->get_member("collection");
                DomView querynode = doc->get_member("query");
                DomView limitnode = doc->get_member("limit");

                if ((!colnode.is_string()) || (!querynode.is_string()) || (!limitnode.is_numeric()))
                {
                    result = "{\"error\":\"collection and document must be of type string. limit must be an unsigned integer.\"}";
                    goto cleanup;
                }

                auto col = get_collection(colnode.get_string());
                if(limitnode.get_numeric_as_double() < 0.5) {
                    col->drop(querynode.get_string(), SIZE_MAX);
                }
                else {
                    col->drop(querynode.get_string(), limitnode.get_numeric_as_double()+0.5);
                }
                result = "{\"msg\":\"OK\"}";

            }
            else
            {
                result = "{\"error\":\"Collection, query and limit fields are required\"}";
            }
        }

        if (cmd == "getValue")
        {
            if (doc->has_member("key"))
            {
                DomView keynode = doc->get_member("key");

                if (!keynode.is_string())
                {
                    result = "{\"error\":\"key field must be a string\"}";
                    goto cleanup;
                }

                std::string query = "{k:\"";
                query += keynode.get_string();
                query += "\"}";
                result = kvstore->find_one(query.c_str());
            }
            else
            {
                result = "{\"error\":\"key field is required\"}";
            }
        }

        if (cmd == "setValue")
        {
            if (doc->has_member("key") && doc->has_member("value"))
            {
                DomView keynode = doc->get_member("key");
                DomView valuenode = doc->get_member("value");

                DomNode *newdoc = dom_node_allocator.make();
                newdoc->set_object();

                DomNode *value = dom_node_allocator.make();
                value->copy(&valuenode);
                newdoc->add_member("v", value);

                DomNode *key = dom_node_allocator.make();
                key->copy(&keynode);
                newdoc->add_member("k", key);

                kvstore->append(newdoc);
                result = "{\"msg\":\"OK\"}";
            }
            else
            {
                result = "{\"error\":\"key and value fields are required\"}";
            }
        }

        cleanup:
            dom_node_allocator.free(doc);
            return result;
    }

    void Database::save_json_collection(ofstream &ofile, Collection* col) const
    {
        ofile << "\"" << col->get_name() << "\":[";
        auto cursor = col->find("{}");
        while (cursor->has_next())
        {
            ofile << cursor->next();
            if (cursor->has_next())
            {
                ofile << ",";
            }
        }
        ofile << "]";
    }
}
