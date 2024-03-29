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

    std::string Collection::find_one(const std::string& query)
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
                return std::string(sb.GetString(), sb.GetSize());
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

        while ((ret < limit) && (nodes.size() > 0))
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

        if (jw.get_enabled()) {

            rapidjson::StringBuffer sb;
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            writer.StartObject();
            writer.Key("cmd");
            writer.String("drop");
            writer.Key("collection");
            writer.String(name.c_str());
            writer.Key("query");
            writer.String(query.c_str());
            writer.Key("limit");
            writer.Int(limit);

            writer.EndObject();
            jw.append(sb.GetString());
        }
        return ret;
    }

    void Collection::upsert(const std::string &query, DomNode *replacement)
    {
        drop(query);
        append(replacement);
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

    bool Collection::update(const std::string& query, const std::string& json)
    {
        Parser parser;
        DomNode *dom = parser.parse(json);
        if (dom == nullptr)
        {
            return false;
        }
        else
        {
            update(query, dom);
            dom_node_allocator.delete_object(dom);
            return true;
        }
    }

    void Collection::update(const std::string &query, DomNode* node) 
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
                merge_docs(*iter->second, node);
                return;
            }
            else {
                iter++;
            }
        }

        if (jw.get_enabled())
        {
            rapidjson::StringBuffer sb;
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            writer.StartObject();
            writer.Key("cmd");
            writer.String("update");
            writer.Key("collection");
            writer.String(name.c_str());
            writer.Key("query");
            writer.String(query.c_str());
            writer.Key("document");

            rapidjson::StringBuffer sbdoc;
            rapidjson::Writer<rapidjson::StringBuffer> docwriter(sbdoc);
            node->stringify(docwriter);
            writer.String(sbdoc.GetString());

            writer.EndObject();
            jw.append(sb.GetString());
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

            rapidjson::StringBuffer sbdoc;
            rapidjson::Writer<rapidjson::StringBuffer> docwriter(sbdoc);
            node->stringify(docwriter);
            writer.String(sbdoc.GetString());

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

    void Collection::merge_docs(DomNode* dst, const DomView* src)
    {
        for(auto iter = src->member_begin(); iter != src->member_end(); iter++)
        {
            if(dst->has_member(iter.get_key()) == false)
            {
                DomNode* other = dom_node_allocator.make();
                other->copy(&(iter.get_value()));
                dst->add_member(iter.get_key(), other);
            }
            else
            {
                if(iter.get_value().get_type() == DOM_NODE_TYPE_OBJECT)
                {
                    merge_docs(dst->get_member_node(iter.get_key()), &iter.get_value());
                }
                else
                {
                    dst->remove_member(iter.get_key());

                    DomNode* other = dom_node_allocator.make();
                    other->copy(&(iter.get_value()));
                    dst->add_member(iter.get_key(), other);
                }
            }
        }
    }

}
