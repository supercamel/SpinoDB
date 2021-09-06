#ifndef SPINODB_INCLUDED
#define SPINODB_INCLUDED

#include <napi.h>
#include <iostream>
#include <vector>
#include <list>
#include <ctime>
#include <string>
#include <map>
#include <fstream>
#include <chrono>
#include <thread>


#include "QueryNodes.h"
#include "QueryParser.h"
#include "QueryExecutor.h"


namespace Spino {

	class Cursor {
		public:
			Cursor(ValueType& list, const char* query) : list(list) { 
				Spino::QueryParser parser(query);
				head = parser.parse_expression();
				iter = list.Begin();
			}

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


	class Collection {
		public:
			Collection(DocType& doc, std::string name) : doc(doc), name(name) {
				id_counter = 0;	
			}

			std::string get_name() const;

			void append(const char* s);
			void update(const char* search, const char* update);

			std::string findOneById(const char* id) const;
			std::string findOne(const char* s);
			shared_ptr<Cursor> find(const char* s) const;

			void dropOne(const char* s);
			void drop(const char* s, bool onlyOne = false);

		private:
			bool mergeObjects(ValueType& dstObject, ValueType& srcObject);

			uint32_t fnv1a_hash(std::string& s);
			uint32_t fast_atoi_len(const char * str, uint32_t len) const
			{
				uint32_t val = 0;
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
			uint32_t last_append_timestamp = 0;
	};


	class SpinoDB {
		public:
			SpinoDB() {
				doc.SetObject();
			}

			std::shared_ptr<Collection> add_collection(std::string name);
			std::shared_ptr<Collection> get_collection(std::string name) const;
			void drop_collection(std::string name);

			void save(std::string path) const;
			void load(std::string path);

		private:
			std::vector<std::shared_ptr<Collection>> collections;
			DocType doc;
	};


}


#endif


