#ifndef SPINODB_INCLUDED
#define SPINODB_INCLUDED

#include <iostream>
#include <vector>
#include <list>
#include <ctime>
#include <string>
#include <fstream>

#include "QueryNodes.h"
#include "QueryParser.h"
#include "QueryExecutor.h"

namespace Spino {

	class Cursor {
		public:
			Cursor(ValueType& list, std::string query) : list(list) { 
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
			void update(std::string search, const char* update);

			std::string findOne(std::string s) const;
			shared_ptr<Cursor> find(std::string s) const;

			void dropOne(std::string s);
			void drop(std::string s, bool onlyOne = false);

		private:
			bool mergeObjects(ValueType& dstObject, ValueType& srcObject);

			uint32_t id_counter;
			std::string name;
			DocType& doc;
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


