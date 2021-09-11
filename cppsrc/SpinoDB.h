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


#include "QueryNodes.h"
#include "QueryParser.h"
#include "QueryExecutor.h"


namespace Spino {

	class BaseCursor {
		public:
            virtual ~BaseCursor() { };
			virtual std::string next() = 0;
	};


	class LinearCursor : public BaseCursor {
		public:
			LinearCursor(ValueType& list, const char* query) : list(list) { 
				Spino::QueryParser parser(query);
				head = parser.parse_expression();
				iter = list.Begin();
			}

            ~LinearCursor() { }

			std::string next() {
				while(iter != list.End()) {
					exec.set_json(&(*iter));	
					if(exec.resolve(head)) {
						rapidjson::StringBuffer sb;
						rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
						iter->Accept(writer);
						iter++;
						return sb.GetString();
					}
					iter++;
				} 
				return "";
			}

		private:
			ValueType& list;
			QueryExecutor exec;
			ValueType::ConstValueIterator iter;
			std::shared_ptr<QueryNode> head;
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
			}

            ~IndexCursor() { }

			std::string next() {
				if(iter != iter_range.second) {
					rapidjson::StringBuffer sb;
					rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
					collection_dom[iter->second].Accept(writer);
					iter++;
					return sb.GetString();
				} 
				return "";
			}

		private:
			ValueType& collection_dom;
			IndexIteratorRange iter_range;
			std::multimap<Spino::Value, uint32_t>::iterator iter;
	};


	class Collection {
		public:
			Collection(DocType& doc, std::string name) : name(name), doc(doc), arr(doc[name.c_str()])  {
				id_counter = 0;	
			}

            ~Collection() {
                for(auto i : indices) {
                    delete i;
                }
            }

			std::string getName() const;

			void createIndex(const char* field);

			void append(ValueType& d);
			void append(const char* s);

			void updateById(const char* id, const char* update);
			void update(const char* search, const char* update);

			std::string findOneById(const char* id) const;
			std::string findOne(const char* s);
			BaseCursor* find(const char* s) const;

			void dropById(const char* s);
			void dropOne(const char* s);
			void drop(const char* s, bool onlyOne = false);

			static uint64_t timestampById(const char* id);

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
			ValueType& arr;
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

			Collection* add_collection(std::string name);
			Collection* get_collection(std::string name) const;
			void drop_collection(std::string name);

			void save(std::string path) const;
			void load(std::string path);

		private:
			std::vector<Collection*> collections;
			DocType doc;
	};


}


#endif


