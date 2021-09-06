#include "SpinoDB.h"

#include <iostream>
#include <random>

using namespace std;

namespace Spino{
	std::string Collection::get_name() const {
		return name;
	}

	void Collection::append(const char* s) {
		auto d = std::make_shared<DocType>();
		d->Parse(s);

		std::string id = std::to_string(std::time(0));

		std::string id_counter_str = std::to_string(id_counter++);
		id_counter_str = std::string(4 - id_counter_str.length(), '0') + id_counter_str;
		if(id_counter >= 10000) {
			id_counter = 0;
		}

		id += id_counter_str;

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> distr(0, 9999);

		id += std::to_string(distr(gen));

		ValueType _id;
		_id.SetString(id.c_str(), id.length(), doc.GetAllocator());

		d->AddMember("_id", _id, doc.GetAllocator());
		doc[name.c_str()].PushBack(d->GetObject(), doc.GetAllocator());
	}

	void Collection::update(std::string search, const char* update) {
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
	}

	std::string Collection::findOne(std::string s) const {
		auto cursor = find(s);
		return cursor->next();
	}

	shared_ptr<Cursor> Collection::find(std::string s) const {
		return make_shared<Cursor>(doc[name.c_str()], s);
	}

	void Collection::dropOne(std::string j) {
		drop(j, true);
	}

	void Collection::drop(std::string j, bool onlyOne) {
		Spino::QueryParser parser(j);
		auto block = parser.parse_expression();

		auto& arr = doc[name.c_str()];
		for (ValueType::ConstValueIterator itr = arr.Begin();
				itr != arr.End(); ++itr) {
			Spino::QueryExecutor exec(&(*itr));
			if(exec.resolve(block)) {
				itr = arr.Erase(itr);
				if(onlyOne) {
					break;
				}
			} else {
				++itr;
			}
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


