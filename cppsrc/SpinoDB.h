//  Copyright 2021 Sam Cowen <samuel.cowen@camelsoftware.com>
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


#ifndef SPINODB_INCLUDED
#define SPINODB_INCLUDED

#include <vector>
#include <list>
#include <ctime>
#include <string>
#include <map>
#include <fstream>
#include <chrono>
#include <thread>
#include <sstream>

#include <iostream>

#include "QueryNodes.h"
#include "QueryParser.h"
#include "QueryExecutor.h"


namespace Spino {

    class BaseCursor {
        public:
            BaseCursor() {
                max_results = UINT32_MAX;
                projection_set = false;
            }

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
                    rapidjson::Writer<rapidjson::StringBuffer>& writer) 
            {
                writer.StartObject();

                // white list projection only
                for (ValueType::ConstMemberIterator itr = proj.MemberBegin();
                        itr != proj.MemberEnd(); ++itr)
                {
                    std::string name = itr->name.GetString();

                    auto srcIt = source.GetObject().FindMember(name.c_str()); 
                    // if the source object has the projection field
                    if(srcIt != source.MemberEnd()) {
                        writer.Key(name.c_str());
                        if(srcIt->value.IsObject() && itr->value.IsObject()) {
                            apply_projection(
                                    itr->value.GetObject(), srcIt->value, writer);

                        }
                        else {
                            srcIt->value.Accept(writer);
                        }
                    }
                }

                writer.EndObject();
            }


            DocType projection;
            bool projection_set;
            uint32_t max_results;
    };


    class LinearCursor : public BaseCursor {
        public:
            LinearCursor(ValueType& list, const char* query) : list(list) { 
                Spino::QueryParser parser(query);
                head = parser.parse_expression();
                iter = list.Begin();
                has_next = true;
                findNext();
            }

            ~LinearCursor() { }

            bool hasNext() {
                return has_next;
            }

            std::string next() {
                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                if(projection_set) {
                    apply_projection(projection, *iter, writer);
                }
                else {
                    iter->Accept(writer);
                }
                std::string ret = buffer.GetString();

                iter++;
                findNext();
                return ret;
            }

            uint32_t count() {
                uint32_t r = 0;
                auto itr = list.Begin();
                while(itr != list.End()) {
                    itr++;
                    exec.set_json(&(*itr));
                    if(exec.resolve(head)) {
                        r++;
                    }
                }

                return r;
            }

        private:
            void findNext() {
                has_next = false;
                if(counter < max_results) {
                    while(iter != list.End()) {
                        exec.set_json(&(*iter));	
                        if(exec.resolve(head)) {
                            has_next = true;
                            counter++;
                            return;
                        }
                        else {
                            iter++;
                        }
                    }               
                }
            }

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
            IndexCursor(IndexIteratorRange iter_range, ValueType& collection_dom) : 
                collection_dom(collection_dom),
                iter_range(iter_range)
        {
            iter = iter_range.first;
            next();
        }

            ~IndexCursor() { }

            bool hasNext() {
                if((counter < max_results) && (iter != iter_range.second)) {
                    return true;
                }
                else {
                    return false;
                }
            }

            std::string next() {
                if(counter < max_results) {
                    if(iter != iter_range.second) {
                        rapidjson::StringBuffer buffer;
                        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                        if(projection_set) {
                            apply_projection(projection, 
                                    collection_dom[iter->second], writer);
                        }
                        else {
                            collection_dom[iter->second].Accept(writer);
                        }
                        iter++;
                        counter++;
                        return buffer.GetString();
                    } 
                }
                return "";
            }

            uint32_t count() {
                uint32_t r = 0;
                auto itr = iter_range.first;
                while(itr != iter_range.second) {
                    itr++;
                    r++;
                }	
                return r;
            }

        private:
            ValueType& collection_dom;
            IndexIteratorRange iter_range;
            std::multimap<Spino::Value, uint32_t>::iterator iter;
            uint32_t counter = 0;
            string nextdoc;
    };


    class Collection {
        public:
            Collection(DocType& doc, std::string name) : name(name), doc(doc)  {
                auto& arr = doc[name.c_str()];
                if(arr.IsArray() == false) {
                    std::cout << "WARNING: collection " 
                        << name 
                        << " is not an array. The database is corrupt." 
                        << std::endl;
                }
                id_counter = 0;	
                last_append_timestamp = std::time(0); 
            }

            ~Collection() {
                for(auto i : indices) {
                    delete i;
                }
            }

            std::string getName() const;

            void createIndex(const char* field);
            void dropIndex(const char* field);

            void append(ValueType& d);
            void append(const char* s);

            void updateById(const char* id, const char* update);
            void update(const char* search, const char* update);

            std::string findOneById(const char* id) const;
            std::string findOne(const char* s);
            BaseCursor* find(const char* s) const;

            void dropById(const char* s);
            void dropOne(const char* s);
            uint32_t drop(const char* s, uint32_t limit = UINT32_MAX);
            uint32_t dropOlderThan(uint64_t timestamp); //milliseconds since 1970 epoch

            static uint64_t timestampById(const char* id);

            uint32_t size() {
                auto& arr = doc[name.c_str()];
                return arr.Size();
            }

        private:
            class Index {
                public:
                    std::string field_name;
                    PointerType field;
                    std::multimap<Spino::Value, uint32_t> index;
            };


            void indexNewDoc();
            void removeDomIdxFromIndex(uint32_t domIdx);
            bool domIndexFromId(const char* s, uint32_t& domIdx) const;
            void reconstructIndices();

            std::vector<Index*> indices;
            bool mergeObjects(ValueType& dstObject, ValueType& srcObject);

            uint32_t fnv1a_hash(std::string& s);
            static uint64_t fast_atoi_len(const char * str, uint32_t len)
            {
                uint64_t val = 0;
                uint32_t count = 0;
                while(count++ < len) {
                    val = val*10 + (*str++ - '0');
                }
                return val;
            }

            uint32_t id_counter;
            std::string name;
            DocType& doc;
            std::map<uint32_t, std::string> hashmap;

            const uint32_t FNV_PRIME = 16777619u;
            const uint32_t OFFSET_BASIS = 2166136261u;
            uint64_t last_append_timestamp = 0;
    };


    class SpinoDB {
        public:
            SpinoDB() {
                doc.SetObject();
            }

            ~SpinoDB() {
                for(auto c : collections) {
                    delete c;
                }
            }

            std::string execute(const char* command);

            Collection* addCollection(std::string name);
            Collection* getCollection(std::string name);
            void dropCollection(std::string name);

            void save(const std::string& path) const;
            void load(const std::string& path);

        private:
            static std::string make_reply(bool success, std::string msg) {
                std::stringstream ss;
                if(success) {
                    ss << "{\"msg\":\"" << msg << "\"}";
                }
                else {
                    ss << "{\"error\":\"" << msg << "\"}";
                }
                return ss.str();
            }

            std::string require_fields(DocType& d, std::vector<std::string> fields) {
                for(auto f : fields) {
                    if(!d.HasMember(f.c_str())) {
                        return make_reply(false, "Missing field " + f);
                    }
                }	
                return "";
            }

            std::vector<Collection*> collections;
            DocType doc;
    };

    std::string escape(const std::string& str);

}


#endif


