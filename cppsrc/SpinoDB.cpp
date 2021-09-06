#include "SpinoDB.h"

#include <iostream>
#include <functional>

using namespace std;

namespace Spino{
	std::string Collection::get_name() const {
		return name;
	}

	void Collection::append(const char* s) {
		auto d = std::make_shared<DocType>();
		d->Parse(s);

		uint32_t timestamp = std::time(0);
		std::string id = std::to_string(timestamp);
		id = std::string(10 - id.length(), '0') + id;

		std::string id_counter_str = std::to_string(id_counter++);
		id_counter_str = std::string(6 - id_counter_str.length(), '0') + id_counter_str;
		if(last_append_timestamp - timestamp != 0) {
			id_counter = 0;
		}

		id += id_counter_str;

		ValueType _id;
		_id.SetString(id.c_str(), id.length(), doc.GetAllocator());

		d->AddMember("_id", _id, doc.GetAllocator());
		doc[name.c_str()].PushBack(d->GetObject(), doc.GetAllocator());
	}

	void Collection::update(const char* search, const char* update) {
		DocType j;
		j.Parse(update);

		Spino::QueryParser parser(search);
		auto block = parser.parse_expression();


		auto& arr = doc[name.c_str()];
		for (ValueType::ValueIterator itr = arr.Begin();
				itr != arr.End(); ++itr) {
			Spino::QueryExecutor exec(&(*itr));
			if(exec.resolve(block)) {
				mergeObjects(*itr, j.GetObject());
			} else {
				++itr;
			}
		}
		hashmap.clear();
	}

	/**
	 * The _id field is gauranteed to be ordered so we can do a binary search
	 */
	std::string Collection::findOneById(const char* id_cstr) const {
		uint32_t tsc = fast_atoi_len(id_cstr, 10);
		uint32_t countc = fast_atoi_len(&id_cstr[10], 6);

		auto& list = doc[name.c_str()];
		auto n = list.Size();
		auto R = n-1;
		uint32_t L = 0;
		while(L <= R) {
			auto m = (L+R)/2;

			const char* id_to_test = list[m].GetObject()["_id"].GetString();
			uint32_t timestamp = fast_atoi_len(id_to_test, 10);

			if(timestamp < tsc) {
				L = m+1;
			}
			else if(timestamp > tsc) {
				R = m-1;
			}
			else {
				uint32_t count = fast_atoi_len(&id_to_test[10], 6);
				if(count < countc) {
					L = m+1;
				} 
				else if(count > countc) {
					R = m-1;
				}
				else {
					rapidjson::StringBuffer sb;
					rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
					list[m].Accept(writer);
					return sb.GetString();
				}
			}
		}
		return "";
	}

	std::string Collection::findOne(const char* s) {
		std::string str = s;
		auto hash = fnv1a_hash(str);
		auto it = hashmap.find(hash);
		if (it != hashmap.end()) {
			return it->second;
		}

		auto cursor = find(s);
		std::string v = cursor->next();
		if(v != "") {
			hashmap[hash] = v;
		}
		return v;
	}

	uint32_t Collection::fnv1a_hash(std::string& s) {
		auto length = s.length()+1;
		unsigned int hash = OFFSET_BASIS;
		for (size_t i = 0; i < length; ++i)
		{
			hash ^= s[i]++;
			hash *= FNV_PRIME;
		}
		return hash;
	}

	shared_ptr<Cursor> Collection::find(const char* s) const {
		return make_shared<Cursor>(doc[name.c_str()], s);
	}

	void Collection::dropOne(const char* j) {
		drop(j, true);
	}

	void Collection::drop(const char* j, bool onlyOne) {
		Spino::QueryParser parser(j);
		auto block = parser.parse_expression();

		auto& arr = doc[name.c_str()];
		bool document_dropped = false;
		for (ValueType::ConstValueIterator itr = arr.Begin();
				itr != arr.End(); ++itr) {
			Spino::QueryExecutor exec(&(*itr));
			if(exec.resolve(block)) {
				itr = arr.Erase(itr);
				document_dropped = true;
				if(onlyOne) {
					break;
				}
			} else {
				++itr;
			}
		}

		if(document_dropped) {
			hashmap.clear();
		}
	}

	bool Collection::mergeObjects(ValueType& dstObject, ValueType& srcObject)
	{
		for (auto srcIt = srcObject.MemberBegin(); srcIt != srcObject.MemberEnd(); ++srcIt)
		{
			auto dstIt = dstObject.FindMember(srcIt->name);
			if (dstIt == dstObject.MemberEnd())
			{
				ValueType dstName ;
				dstName.CopyFrom(srcIt->name, doc.GetAllocator());
				ValueType dstVal ;
				dstVal.CopyFrom(srcIt->value, doc.GetAllocator());

				dstObject.AddMember(dstName, dstVal, doc.GetAllocator());

				dstName.CopyFrom(srcIt->name, doc.GetAllocator());
				dstIt = dstObject.FindMember(dstName);
				if (dstIt == dstObject.MemberEnd())
					return false ;
			}
			else
			{
				auto srcT = srcIt->value.GetType() ;
				auto dstT = dstIt->value.GetType() ;
				if(srcT != dstT)
					return false ;

				if (srcIt->value.IsArray())
				{
					for (auto arrayIt = srcIt->value.Begin(); arrayIt != srcIt->value.End(); ++arrayIt)
					{
						ValueType dstVal ;
						dstVal.CopyFrom(*arrayIt, doc.GetAllocator()) ;
						dstIt->value.PushBack(dstVal, doc.GetAllocator());
					}
				}
				else if (srcIt->value.IsObject())
				{
					if(!mergeObjects(dstIt->value, srcIt->value))
						return false ;
				}
				else
				{
					dstIt->value.CopyFrom(srcIt->value, doc.GetAllocator()) ;
				}
			}
		}

		return true ;
	}

	std::shared_ptr<Collection> SpinoDB::add_collection(std::string name) {
		for(auto i : collections) {
			if(i->get_name() == name) {
				return nullptr;
			}
		}

		ValueType v(rapidjson::kArrayType);
		ValueType index;
		index.SetString(name.c_str(), name.size(), doc.GetAllocator());
		doc.AddMember(index, v, doc.GetAllocator());

		auto c = std::make_shared<Collection>(doc, name);
		collections.push_back(c);
		return c;
	}

	std::shared_ptr<Collection> SpinoDB::get_collection(std::string name) const {
		for(auto c : collections) {
			if(c->get_name() == name) {
				return c;
			}
		}
		return nullptr;
	}

	void SpinoDB::drop_collection(std::string name) {
		for(auto it = collections.begin(); it != collections.end(); ) {
			auto c = *it;
			if(c->get_name() == name) {
				collections.erase(it);
			} else {
				it++;
			}
		}
	}

	void SpinoDB::save(std::string path) const {
		std::ofstream out(path);
		rapidjson::OStreamWrapper osw(out);

		rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
		doc.Accept(writer);
		out.close();
	}

	void SpinoDB::load(std::string path) {
		collections.clear();

		std::ifstream in(path);
		rapidjson::IStreamWrapper isw(in);

		doc.ParseStream(isw);

		for (auto& m : doc.GetObject()) {
			auto c = make_shared<Collection>(doc, m.name.GetString());
			collections.push_back(c);
		}

	}
}


