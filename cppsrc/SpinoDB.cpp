#include "SpinoDB.h"

#include <iostream>
#include <functional>

using namespace std;

namespace Spino{
	std::string Collection::get_name() const {
		return name;
	}

	std::string Collection::append(const char* s) {
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

		last_append_timestamp = timestamp;

		id += id_counter_str;

		ValueType _id;
		_id.SetString(id.c_str(), id.length(), doc.GetAllocator());

		d->AddMember("_id", _id, doc.GetAllocator());
		ValueType& arr = doc[name.c_str()].PushBack(d->GetObject(), doc.GetAllocator());

		ValueType& newdoc = arr[arr.Size()-1];

		for(auto& idx : indices) {
			auto v = idx->field.Get(newdoc);
			if(v) {
				if(v->IsString()) {
					Value val;
					val.type = TYPE_STRING;
					val.str = v->GetString();
					idx->index.insert({val, arr.Size()-1});
				} else if(v->IsNumber()) {
					Value val;
					val.type = TYPE_NUMERIC;
					val.numeric = v->GetDouble();
					idx->index.insert({val, arr.Size()-1});
				}
			}
		}
		return id;
	}

	void Collection::updateById(const char* id_cstr, const char* update) {
		uint32_t domIdx;
		if(domIndexFromId(id_cstr, domIdx)) {
			DocType j;
			j.Parse(update);

			auto& arr = doc[name.c_str()];
			mergeObjects(arr[domIdx], j.GetObject());
			hashmap.clear();
		}
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

	void Collection::create_index(const char* s) {
		auto idx = make_shared<Collection::Index>();;
		idx->field_name = s;
		stringstream ss(s);
		string intermediate;
		string ptr;
		while(getline(ss, intermediate, '.')) {
			ptr += "/" + intermediate;
		}
		idx->field = PointerType(ptr.c_str());

		auto& list = doc[name.c_str()];
		auto n = list.Size();

		for(uint32_t i = 0; i < n; i++) {
			ValueType& doc = list[i].GetObject();
			auto v = idx->field.Get(doc);

			// only add string and number values to the index
			if(v->IsString()) {
				Value val;
				val.type = TYPE_STRING;
				val.str = v->GetString();
				idx->index.insert({val, n});
			}
			else if(v->IsNumber()) {
				Value val;
				val.type = TYPE_NUMERIC;
				val.numeric = v->GetDouble();
				idx->index.insert({val, n});
			} else {
				// can't index objects or arrays, etc
			}
		}

		indices.push_back(idx);
	}

	/**
	 * The _id field is gauranteed to be ordered so we can do a binary search
	 */
	std::string Collection::findOneById(const char* id_cstr) const {
		uint32_t m;
		if(domIndexFromId(id_cstr, m)) {
			auto& list = doc[name.c_str()];
			rapidjson::StringBuffer sb;
			rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
			list[m].Accept(writer);
			return sb.GetString();
		}
		return "";
	}

	bool Collection::domIndexFromId(const char* id_cstr, uint32_t& domIdx) const {
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
					domIdx = m;
					return true;
				}
			}
		}
		return false;
	}

	std::string Collection::findOne(const char* s) {
		std::string v; //result

		//check hashmap for existing query results
		std::string str = s;
		auto hash = fnv1a_hash(str);
		auto it = hashmap.find(hash);
		if (it != hashmap.end()) {
			return it->second;
		}

		//check if it's an index search
		QueryParser parser(s);
		std::shared_ptr<BasicFieldComparison> bfc = nullptr;
		try {
			bfc = parser.parse_basic_comparison();
		} catch(parse_error e) {

		}
		if(bfc != nullptr) {
			for(auto& idx : indices) {
				if(idx->field_name == bfc->field_name) {
					auto iter = idx->index.find(bfc->v);
					if(iter != idx->index.end()) {
						auto n = iter->second;

						rapidjson::StringBuffer sb;
						rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
						doc[name.c_str()][n].Accept(writer);
						v = sb.GetString();
					}
					break;
				}
			}
		}

		//if it's not an index search, do a linear search using a cursor
		if(v == "") {
			auto cursor = find(s);
			v = cursor->next();
		}

		//if there is a result, add it to the hashmap for future reference
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

	shared_ptr<BaseCursor> Collection::find(const char* s) const {
		//check if it's an index search
		QueryParser parser(s);
		std::shared_ptr<BasicFieldComparison> bfc = nullptr;
		try {
			bfc = parser.parse_basic_comparison();
		} catch(parse_error e) {

		}

		if(bfc != nullptr) {
			for(auto& idx : indices) {
				if(idx->field_name == bfc->field_name) {
					auto range = idx->index.equal_range(bfc->v);
					return make_shared<IndexCursor>(range, doc[name.c_str()]);
				}
			}
		}
		return make_shared<LinearCursor>(doc[name.c_str()], s);
	}

	void Collection::removeDomIdxFromIndex(uint32_t domIdx) {
		ValueType& dom_object = doc[name.c_str()][domIdx];

		for(auto idx : indices) {
			std::vector<std::pair<Spino::Value, int>> idx_copies;
			auto iitr = idx->index.begin();
			while(iitr != idx->index.end()) {
				auto entry = *iitr;
				if(entry.second == domIdx) {
					iitr = idx->index.erase(iitr);
				} 
				else if(entry.second > domIdx) {
					idx_copies.push_back({iitr->first, iitr->second-1});
					iitr = idx->index.erase(iitr);
				}
				else {	
					iitr++;
				}
			}

			for(auto i : idx_copies) {
				idx->index.insert(i);
			}
		}
	}

	void Collection::dropById(const char* s) {
		uint32_t domIdx;
		if(domIndexFromId(s, domIdx)) {
			removeDomIdxFromIndex(domIdx);

			auto& arr = doc[name.c_str()];
			auto iter = arr.Begin();
			iter += domIdx;

			doc[name.c_str()].Erase(iter);
			hashmap.clear();
		}
	}


	void Collection::dropOne(const char* j) {
		drop(j, true);
	}

	void Collection::drop(const char* j, bool onlyOne) {
		Spino::QueryParser parser(j);
		auto block = parser.parse_expression();

		auto& arr = doc[name.c_str()];
		bool document_dropped = false;
		uint32_t count = 0;
		for (ValueType::ConstValueIterator itr = arr.Begin();
				itr != arr.End(); ++itr) {
			Spino::QueryExecutor exec(&(*itr));
			if(exec.resolve(block)) {
				removeDomIdxFromIndex(count);
				itr = arr.Erase(itr);
				document_dropped = true;
				if(onlyOne) {
					break;
				}
			} else {
				++itr;
			}
			count++;
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


