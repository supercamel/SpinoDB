#include "collection.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

namespace Spino
{
    Collection::Collection(const std::string &name, JournalWriter &jw) : name(name), jw(jw)
    {
        indices.push_back({"_id"});
        id = 0;
    }

    Collection::~Collection()
    {
        while (nodes.size())
        {
            auto *node = nodes.back();
            dom_node_allocator.delete_object(node);
            nodes.pop_back();
        }
    }

    void Collection::create_index(const std::string &name)
    {
        indices.push_back({name});

        auto &index = indices[indices.size() - 1];
        size_t count = 0;

        NodeListIterator iter = nodes.begin();
        while (iter != nodes.end())
        {
            if ((*iter)->has_member(name))
            {
                auto &member = (*iter)->get_member(name);
                index.index.insert({member, iter});
                count++;
            }
            iter++;
        }
    }

    void Collection::drop_index(const std::string &name)
    {
        for (auto &idx : indices)
        {
            if (idx.field_name == name)
            {
                indices.erase(indices.begin() + (&idx - &indices[0]));
                return;
            }
        }
    }

    const char *Collection::find_one(const char *query)
    {
        QueryParser query_parser(indices, query);
        IndexIteratorRange range;
        auto instructions = query_parser.parse_query(range);
        executor.set_instructions(instructions);
        auto iter = range.first;
        while (iter != range.second)
        {
            if(executor.execute_query(*iter->second) == true)
            {
                sb.Clear();
                writer.Reset(sb);
                (*iter->second)->stringify(writer);
                return sb.GetString();
            }
            else {
                iter++;
            }
        }
        return "";
    }

    DomView *Collection::find_one_dom(const std::string &query)
    {
        auto cursor = find(query);
        DomView *next = cursor->next_dom();
        return next;
    }

    DomView *Collection::find_one_dom(const std::string &index_key, const DomView &value)
    {
        for (auto &idx : indices)
        {
            if (idx.field_name == index_key)
            {
                auto result = idx.index.find(value);
                if (result != idx.index.end())
                {
                    return *result->second;
                }
            }
        }
        return nullptr;
    }

    shared_ptr<Cursor> Collection::find(const std::string &query)
    {
        return shared_ptr<Cursor>(new Cursor(get_indices(), query));
    }

    size_t Collection::drop(const std::string &query, size_t limit)
    {
        size_t ret = 0;
        bool jwenabled = jw.get_enabled();
        jw.set_enabled(false);

        while (ret < limit && nodes.size())
        {
            auto cursor = find(query);
            if (cursor->has_next())
            {
                auto iter = cursor->get_iter();
                drop_one_by_iter(iter->second);
                ret++;
            }
            else
            {
                break;
            }
        }

        jw.set_enabled(jwenabled);

        if (jw.get_enabled())
        {
            stringstream ss;
            ss << "{\"cmd\":\"drop\",\"collection\":\"";
            ss << escape(name);
            ss << "\",\"query\":\"" << escape(query);
            ss << "\",\"limit\":" << limit << "}";
            jw.append(ss.str());
        }
        return ret;
    }

    void Collection::upsert(const std::string &query, DomNode *replacement)
    {
        drop(query, 1);
        append(replacement);

        if (jw.get_enabled())
        {
            rapidjson::StringBuffer sb;
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);

            writer.StartObject();
            writer.Key("cmd");
            writer.String("upsert");
            writer.Key("collection");
            writer.String(name.c_str());
            writer.Key("query");
            writer.String(query.c_str());
            writer.Key("document");
            replacement->stringify(writer);
            writer.EndObject();
            jw.append(sb.GetString());
        }
    }

    bool Collection::upsert(const std::string &query, const std::string &json)
    {
        Parser parser;
        DomNode *dom = parser.parse(json);
        if (dom == nullptr)
        {
            return false;
        }
        else
        {
            upsert(query, dom);
            return true;
        }
    }

    void Collection::append(DomNode *node)
    {
        if (node->get_type() != DOM_NODE_TYPE_OBJECT)
        {
            cout << "Error: Can only append objects to collections" << endl;
            return;
        }

        if (node->has_member("_id") == false)
        {
            DomNode *id_node = dom_node_allocator.make();
            id_node->set_uint(id++);
            node->add_member("_id", id_node);
        }
        else
        {
            auto &id_node = node->get_member("_id");
            if (id_node.get_type() == DOM_NODE_TYPE_UINT)
            {
                if (id_node.get_uint() >= id)
                {
                    id = id_node.get_uint() + 1;
                }
            }
            else
            {
                cout << "Error: _id must be an unsigned integer" << endl;
                return;
            }
        }

        nodes.emplace_back(node);

        auto end = nodes.end();
        end--;

        for (auto &index : indices)
        {
            if (node->has_member(index.field_name))
            {
                DomView member = node->get_member(index.field_name);
                index.index.insert(std::make_pair(member, end));
            }
        }

        if (jw.get_enabled())
        {
            rapidjson::StringBuffer sb;
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            writer.StartObject();
            writer.Key("cmd");
            writer.String("append");
            writer.Key("collection");
            writer.String(name.c_str());
            writer.Key("document");
            node->stringify(writer);
            writer.EndObject();
            jw.append(sb.GetString());
        }
    }

    bool Collection::append(const std::string &json)
    {
        Parser parser;
        DomNode *node = parser.parse(json);
        if (node == nullptr)
        {
            return false;
        }
        else
        {
            append(node);
            return true;
        }
    }

    size_t Collection::size()
    {
        return nodes.size();
    }

    std::vector<Index>* Collection::get_indices() { return &indices; }

    void Collection::clear()
    {
        for (auto &node : nodes)
        {
            dom_node_allocator.delete_object(node);
        }
        nodes.clear();

        for (auto &index : indices)
        {
            index.index.clear();
        }
    }

    void Collection::drop_one_by_iter(NodeListIterator iter)
    {
        for (auto &index : indices)
        {
            if ((*iter)->has_member(index.field_name))
            {
                DomView value = (*iter)->get_member(index.field_name);
                auto range = index.index.equal_range(value);

                auto it = range.first;
                // iterate range and erase all elements with the same value
                while (it != index.index.end())
                {
                    if (it->second == iter)
                    {
                        it = index.index.erase(it);
                        break;
                    }
                    else if (it == range.second)
                    {
                        break;
                    }
                    else
                    {
                        it++;
                    }
                }
            }
        }

        dom_node_allocator.delete_object(*iter);
        nodes.erase(iter);
    }

    void Collection::to_not_bson(std::ostream &os)
    {
        for (auto &node : nodes)
        {
            os.put(DOM_NODE_TYPE_OBJECT);
            node->to_not_bson(os);
        }
    }

}
