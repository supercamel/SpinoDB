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


#include "SpinoDB.h"
#include "squirrel.h"
#include <functional>

#include <iostream>
using namespace std;

namespace Spino{

    void SpinoDB::clear() {
        for(auto c : collections) {
            delete c;
        }
        if(keyStore != nullptr) {
            delete keyStore;
            keyStore = nullptr;
        }

        doc.SetObject();

        const char* keystoreName = "__SpinoKeyValueStore__";
        ValueType v(rapidjson::kArrayType);
        ValueType index(keystoreName, doc.GetAllocator());
        doc.AddMember(index, v, doc.GetAllocator());

        keyStore = new Collection(doc, jw, keystoreName);
        keyStore->createIndex("k");
    }

    Collection* SpinoDB::addCollection(const std::string& name) {
        for(auto i : collections) {
            if(i->getName() == name) {
                return nullptr;
            }
        }

        ValueType v(rapidjson::kArrayType);
        ValueType index(name.c_str(), doc.GetAllocator());
        doc.AddMember(index, v, doc.GetAllocator());

        auto c = new Collection(doc, jw, name);
        collections.push_back(c);

        if(jw.getEnabled()) {
            stringstream ss;
            ss << "{\"cmd\":\"addCollection\",\"collection\":\"";
            ss << escape(name);
            ss << "\"}";
            jw.append(ss.str());
        }

        return c;
    }

    Collection* SpinoDB::getCollection(const std::string& name) {
        for(auto c : collections) {
            if(c->getName() == name) {
                return c;
            }
        }

        return addCollection(name);
    }

    bool SpinoDB::hasCollection(const std::string& name) const {
        for(auto c : collections) {
            if(c->getName() == name) {
                return true;
            }
        }
        return false;
    }

    uint64_t Collection::timestampById(const char* s) {
        return fast_atoi_len(s, 10)*1000;
    }

    void SpinoDB::dropCollection(const std::string& name) {
        for(auto it = collections.begin(); it != collections.end(); ) {
            auto c = *it;
            if(c->getName() == name) {
                doc.RemoveMember(name.c_str()); //remove from DOM
                delete c; //delete the collection object
                collections.erase(it); //remove from the list
                break;
            } else {
                it++;
            }
        }

        if(jw.getEnabled()) {
            stringstream ss;
            ss << "{\"cmd\":\"dropCollection\",\"collection\":\"";
            ss << escape(name);
            ss << "\"}";
            jw.append(ss.str());
        }
    }


    template <typename T> std::string make_key_value_json(
            const std::string& key, const T& value) {
        std::stringstream ss;
        ss << "{\"k\":\""
            << escape(key) << "\",\"v\":"
            << value << "}";
        return ss.str();
    }

    void SpinoDB::setIntValue(const std::string& key, int value) {
        std::stringstream ss;
        ss << "{k:\"" << escape(key) << "\"}";
        keyStore->update(ss.str().c_str(), make_key_value_json(key, value).c_str());
    }

    void SpinoDB::setUintValue(const std::string& key, unsigned int value) {
        std::stringstream ss;
        ss << "{k:\"" << escape(key) << "\"}";
        keyStore->update(ss.str().c_str(), make_key_value_json(key, value).c_str());
    }

    void SpinoDB::setDoubleValue(const std::string& key, double value) {
        std::stringstream ss;
        ss << "{k:\"" << escape(key) << "\"}";
        keyStore->update(ss.str().c_str(), make_key_value_json(key, value).c_str());
    }

    void SpinoDB::setStringValue(const std::string& key, const std::string& value) {
        std::stringstream query_ss;
        query_ss << "{k:\"" << escape(key) << "\"}";

        std::stringstream ss;
        ss << "\"" << escape(value) << "\"";
        keyStore->update(query_ss.str().c_str(), make_key_value_json(key, ss.str()).c_str());
    }

    int SpinoDB::getIntValue(const std::string& key) {
        std::stringstream ss;
        ss << "{k:\""
            << escape(key) << "\"}";

        std::string result = keyStore->findOne(ss.str().c_str());
        if(result != "") {
            DocType keydoc;
            keydoc.Parse(result.c_str());
            if(keydoc.HasMember("v")) {
                if(keydoc["v"].IsNumber()) {
                    return keydoc["v"].GetInt();
                }
            }
        }
        return 0;
    }

    unsigned int SpinoDB::getUintValue(const std::string& key) {
        std::stringstream ss;
        ss << "{k:\""
            << escape(key) << "\"}";


        std::string result = keyStore->findOne(ss.str().c_str());
        if(result != "") {
            DocType keydoc;
            keydoc.Parse(result.c_str());
            if(keydoc.HasMember("v")) {
                if(keydoc["v"].IsNumber()) {
                    return keydoc["v"].GetUint();
                }
            }
        }
        return 0;
    }

    double SpinoDB::getDoubleValue(const std::string& key) {
        std::stringstream ss;
        ss << "{k:\""
            << escape(key) << "\"}";


        std::string result = keyStore->findOne(ss.str().c_str());
        if(result != "") {
            DocType keydoc;
            keydoc.Parse(result.c_str());
            if(keydoc.HasMember("v")) {
                if(keydoc["v"].IsNumber()) {
                    return keydoc["v"].GetDouble();
                }
            }
        }
        return 0;
    }

    const char* SpinoDB::getStringValue(const std::string& key) {
        std::stringstream ss;
        ss << "{k:\""
            << escape(key) << "\"}";

        std::string result = keyStore->findOne(ss.str().c_str());
        if(result != "") {
            DocType keydoc;
            keydoc.Parse(result.c_str());
            if(keydoc.HasMember("v")) {
                if(keydoc["v"].IsString()) {
                    return keydoc["v"].GetString();
                }
            }
        }
        return "";

    }

    bool SpinoDB::hasKey(const std::string& key) {
        std::stringstream ss;
        ss << "{k:\""
            << escape(key) << "\"}";

        std::string result = keyStore->findOne(ss.str().c_str());
        return result != "";
    }



    std::string SpinoDB::execute(const std::string& command) {
        DocType d;
        d.Parse(command.c_str());
        if(d.HasParseError()) {
            return make_reply(false, "JSON parse error.");
        }

        Collection* col = nullptr;

        if(!d.IsObject()) {
            return make_reply(false, "Bad command. Not a json object.");
        }

        if(!d.HasMember("cmd")) {
            return make_reply(false, "Missing cmd field.");
        }

        auto& cmdValue = d["cmd"];
        if(!cmdValue.IsString()) {
            return make_reply(false, "cmd field must be a string.");
        }

        if(d.HasMember("collection")) {
            auto& collectionValue = d["collection"];
            if(!collectionValue.IsString()) {
                return make_reply(false, "collection field must be a string");
            }

            col = getCollection(collectionValue.GetString());
            if(col == nullptr) {
                return make_reply(false, "collection doesn't exist");
            }
        }

        std::string cmdString = cmdValue.GetString();
        if(cmdString == "count") {
            if(d.HasMember("collection")) {
                std::stringstream ss;
                ss << "{\"r\":1,\"count\":" << col->size() << "}";
                return ss.str();
            }
            else {
                std::stringstream ss;
                ss << "{\"r\":1,\"count\":" << collections.size() << "}";
                return ss.str();
            }
        }

        else if(cmdString == "addCollection") {
            auto check = require_fields(d, {"collection"});
            if(check == "") {
                addCollection(d["collection"].GetString());
            }
            else {
                return check;
            }
        }

        else if(cmdString == "dropCollection") {
            auto check = require_fields(d, {"collection"});
            if(check == "") {
                dropCollection(d["collection"].GetString());
            }
            else {
                return check;
            }
        }

        else if(cmdString == "createIndex") {
            auto check = require_fields(d, {"collection", "field"});
            if(check == "") {
                auto& fieldValue = d["field"];
                if(!fieldValue.IsString()) {
                    return make_reply(false, "Field is not a string");
                }

                col->createIndex(fieldValue.GetString());
                return make_reply(true, "Index created");
            }
            else {
                return check;
            }
        }

        else if(cmdString == "dropIndex") {
            auto check = require_fields(d, {"collection", "field"});
            if(check == "") {
                auto& fieldValue = d["field"];
                if(!fieldValue.IsString()) {
                    return make_reply(false, "Field is not a string");
                }

                col->dropIndex(fieldValue.GetString());
                return make_reply(true, "Index dropped");
            }
            else {
                return check;
            }
        }

        else if(cmdString == "findById") {
            auto check = require_fields(d, {"collection", "id"});
            if(check == "") {
                auto& queryValue = d["id"];
                if(!queryValue.IsString()) {
                    return make_reply(false, "Query field is not a string");
                }

                auto response = col->findOneById(queryValue.GetString());
                return response;
            }
            else {
                return check;
            }
        }
        else if(cmdString == "findOne") {
            auto check = require_fields(d, {"collection", "query"});
            if(check == "") {
                auto& queryValue = d["query"];
                if(!queryValue.IsString()) {
                    return make_reply(false, "Query field is not a string");
                }

                auto response = col->findOne(queryValue.GetString());
                return response;
            }
            else {
                return check;
            }
        }

        else if(cmdString == "find") {
            auto check = require_fields(d, {"collection", "query"});
            if(check == "") {
                auto& queryValue = d["query"];
                if(!queryValue.IsString()) {
                    return make_reply(false, "Query field is not a string");
                }

                uint32_t limit = UINT32_MAX;
                if(d.HasMember("limit")) {
                    auto& limitValue = d["limit"];
                    if(limitValue.IsNumber()) {
                        limit = limitValue.GetInt();
                    }
                }


                auto cursor = col->find(queryValue.GetString())->setLimit(limit);
                std::string response = "[";

                std::string docstr = cursor->next();
                while(docstr != "") {
                    response += docstr;
                    response += ",";
                    docstr = cursor->next();
                }
                if(response.size() > 1) {
                    response.pop_back();
                }
                response += "]";

                delete cursor;
                return response;
            } else {
                return check;
            }
        }

        else if(cmdString == "append") {
            auto check = require_fields(d, {"collection", "document"});
            if(check == "") {
                auto& collectionValue = d["collection"];
                if(!collectionValue.IsString()) {
                    return make_reply(false, "Collection field is not a string");
                }

                auto& documentValue = d["document"];
                if(!documentValue.IsString()) {
                    return make_reply(false, "Document field is not a string");
                }
                col->append(documentValue.GetString());
                return make_reply(true, "Document added");
            }
            else {
                return check;
            }
        }

        else if(cmdString == "updateById") {
            auto check = require_fields(d, {"collection", "id", "document"});
            if(check == "") {
                auto& collectionValue = d["collection"];
                if(!collectionValue.IsString()) {
                    return make_reply(false, "Collection field is not a string");
                }

                auto& queryValue = d["id"];
                if(!queryValue.IsString()) {
                    return make_reply(false, "id must be a string");
                }

                auto& documentValue = d["document"];
                if(!documentValue.IsString()) {
                    return make_reply(false, "Document field is not a string");
                }
                col->updateById(queryValue.GetString(), documentValue.GetString());
                return make_reply(true, "Document updated");
            }
            else {
                return check;
            }
        }

        else if(cmdString == "update") {
            auto check = require_fields(d, {"collection", "query", "document"});
            if(check == "") {
                auto& collectionValue = d["collection"];
                if(!collectionValue.IsString()) {
                    return make_reply(false, "Collection field is not a string");
                }

                auto& queryValue = d["query"];
                if(!queryValue.IsString()) {
                    return make_reply(false, "query must be a string");
                }

                auto& documentValue = d["document"];
                if(!documentValue.IsString()) {
                    return make_reply(false, "Document field is not a string");
                }
                col->update(queryValue.GetString(), documentValue.GetString());
                return make_reply(true, "Document updated");
            }
            else {
                return check;
            }
        }

        else if(cmdString == "dropById") {
            auto check = require_fields(d, {"collection", "id"});
            if(check == "") {
                auto& collectionValue = d["collection"];
                if(!collectionValue.IsString()) {
                    return make_reply(false, "Collection field is not a string");
                }

                auto& queryValue = d["id"];
                if(!queryValue.IsString()) {
                    return make_reply(false, "id must be a string");
                }

                col->dropById(queryValue.GetString());
                return make_reply(true, "Document dropped");
            }
            else {
                return check;
            }
        }

        else if(cmdString == "dropOne") {
            auto check = require_fields(d, {"collection", "query"});
            if(check == "") {
                auto& collectionValue = d["collection"];
                if(!collectionValue.IsString()) {
                    return make_reply(false, "Collection field is not a string");
                }

                auto& queryValue = d["query"];
                if(!queryValue.IsString()) {
                    return make_reply(false, "id must be a string");
                }

                col->dropOne(queryValue.GetString());
                return make_reply(true, "Document dropped");
            }
            else {
                return check;
            }
        }

        else if(cmdString == "drop") {
            auto check = require_fields(d, {"collection", "query"});
            if(check == "") {
                auto& queryValue = d["query"];
                if(!queryValue.IsString()) {
                    return make_reply(false, "id must be a string");
                }

                uint32_t limit = UINT32_MAX;
                if(d.HasMember("limit")) {
                    auto& limitValue = d["limit"];
                    if(!limitValue.IsNumber()) {
                        return make_reply(false, "limit must be a number");
                    }
                    limit = limitValue.GetInt();
                }

                auto r = col->drop(queryValue.GetString(), limit);
                std::string reply = std::to_string(r) + " documents dropped";
                return make_reply(true, reply);
            }
            else {
                return check;
            }
        }

        else if(cmdString == "dropOlderThan") {
            auto check = require_fields(d, {"collection", "timestamp"});
            if(check == "") {
                auto& timestampValue = d["timestamp"];
                if(!timestampValue.IsNumber()) {
                    return make_reply(false, "timestamp must be a number");
                }

                auto r = col->dropOlderThan(timestampValue.GetDouble());
                std::string reply = std::to_string(r) + " documents dropped";
                return make_reply(true, reply);
            }
            else {
                return check;
            }
        }

        else if(cmdString == "getValue") {
            auto check = require_fields(d, {"key"});
            if(check == "") {
                std::string query = "{k:\"";
                query += d["key"].GetString();
                query += "\"}";
                std::string result = keyStore->findOne(query.c_str());
                return result;
            }
            else {
                return check;
            }
        }

        else if(cmdString == "setValue") {
            auto check = require_fields(d, {"key", "value"});
            if(check == "") {
                auto& value = d["value"];
                if(value.IsNumber()) {
                    if(value.IsInt()) {
                        setIntValue(d["key"].GetString(), d["value"].GetInt());
                    }
                    else if(value.IsUint()) {
                        setUintValue(d["key"].GetString(), d["value"].GetUint());
                    }
                    else {
                        setDoubleValue(d["key"].GetString(), d["value"].GetDouble());
                    }
                }
                else if(value.IsString()) {
                    setStringValue(d["key"].GetString(), d["value"].GetString());
                }
                else {
                    return make_reply(false, "Value type can be either a number or a string.");
                }
                return make_reply(true, "Value added");
            }
            else {
                return check;
            }
        }


        return make_reply(false, "Unknown command");
    }


    void SpinoDB::save(const std::string& path) const {
        // dump the json to a temporary file
        std::string tmppath = path + "spinotmp";
        std::ofstream out(tmppath);
        rapidjson::OStreamWrapper osw(out);

        rapidjson::Writer<rapidjson::OStreamWrapper> writer(osw);
        doc.Accept(writer);

        out.flush();
        out.close();

        // move the temporary file to the correct location
        std::rename(tmppath.c_str(), path.c_str());
        std::remove(tmppath.c_str());

        // clear the journal
        if(jw.getEnabled()) {
            std::ofstream ofs;
            ofs.open(jw.getPath(), std::ofstream::out | std::ofstream::trunc);
            ofs.close();
        }
    }

    bool SpinoDB::load(const std::string& path) {
        const char* keystoreName = "__SpinoKeyValueStore__";
        // clean up
        for(auto i : collections) {
            delete i;
        }
        delete keyStore;
        keyStore = nullptr;
        collections.clear();
        doc.SetObject(); // clear the whole dom

        try {
            // load json from file
            std::ifstream in(path);
            rapidjson::IStreamWrapper isw(in);
            doc.ParseStream(isw);
        }
        catch(...) {
            clear();
            return false;
        }

        // create the collections & key store collection
        for (auto& m : doc.GetObject()) {
            std::string name = m.name.GetString();
            if(name != keystoreName) {
                auto c = new Collection(doc, jw, m.name.GetString());
                collections.push_back(c);
            }
        }

        if(doc.HasMember(keystoreName)) {
            keyStore = new Collection(doc, jw, keystoreName);
            keyStore->createIndex("k");
        }
        else {
            ValueType v(rapidjson::kArrayType);
            ValueType index(keystoreName, doc.GetAllocator());
            doc.AddMember(index, v, doc.GetAllocator());

            keyStore = new Collection(doc, jw, keystoreName);
            keyStore->createIndex("k");

        }
        return true;
    }

    void SpinoDB::enableJournal(const std::string& jpth) {
        jw.setPath(jpth);
        jw.setEnabled(true);
    }

    void SpinoDB::disableJournal() {
        jw.setEnabled(false);
    }

    void SpinoDB::consolidate(const std::string& path) {
        bool priorState = jw.getEnabled();
        jw.setEnabled(false);

        // read the journal file and execute the commands
        std::ifstream file(jw.getPath());
        if (file.is_open()) {
            std::string line;
            while (std::getline(file, line)) {
                execute(line);
            }
            file.close();
        }

        jw.setEnabled(priorState);

        // write the db to disk 
        // noting that save will also erase the journal
        save(path);
    }

    /*
       std::string SpinoDB::runScript(const std::string& script) {
       HSQUIRRELVM v;
       v = sq_open(1024); //creates a VM with initial stack size 1024

    //do some stuff with squirrel here
    //
    sq_close(v);
    }
    */


    std::string escape(const std::string& str) {
        std::string result;
        for(char c : str) {
            if(c == '"') {
                result += "\\\"";
            }
            else if(c == '\\') {
                result += "\\\\";
            }
            else if(c == '\n') {
                result += "\\n";
            }
            else if(c == '\t') {
                result += "\\t";
            }
            else {
                result += c;
            }
        }
        return result;
    }

    std::string unescape(const std::string& str) {
        std::string result;
        unsigned int len = str.length();
        unsigned int i = 0;
        while(i < len) {
            char c = str[i++];
            if(c == '\\') {
                c = str[i++];
                if(c == 'n') {
                    result += '\n';
                }
                else if(c == '\\') {
                    result += '\\';
                }
                else if(c == 't') {
                    result += '\t';
                }
                else if(c == '"') {
                    result += '"';
                }
                else {
                    cout << "SpinoDB:: unescape: unknown escape sequence in string.\n" << str << endl;
                    cout << str[i-1] << endl;
                }
            }
            else {
                result += c;
            }
        }
        return result;
    }
}


