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
#include "Cursor.h"
#include "Collection.h"
#include "Journal.h"

namespace Spino {

    class SpinoDB {
        public:
            SpinoDB() {
                doc.SetObject();

                const char* keystoreName = "__SpinoKeyValueStore__";
                ValueType v(rapidjson::kArrayType);
                ValueType index(keystoreName, doc.GetAllocator());
                doc.AddMember(index, v, doc.GetAllocator());

                keyStore = new Collection(doc, jw, keystoreName);
                keyStore->createIndex("k");
            }

            ~SpinoDB() {
                for(auto c : collections) {
                    delete c;
                }
                delete keyStore;
            }

            void clear();

            std::string execute(const std::string& command);

            Collection* addCollection(const std::string& name);
            Collection* getCollection(const std::string& name);
            bool hasCollection(const std::string& name) const;
            vector<string> listCollections() const;
            void dropCollection(const std::string& name);

            void save(const std::string& db_path) const;
            bool load(const std::string& db_path);

            void enableJournal(const std::string& journal_path);
            void disableJournal();
            void consolidate(const std::string& db_path);

            void setBoolValue(const std::string& key, bool value);
            void setIntValue(const std::string& key, int value);
            void setUintValue(const std::string& key, unsigned int value);
            void setDoubleValue(const std::string& key, double value);
            void setStringValue(const std::string& key, const std::string& value);

            bool getBoolValue(const std::string& key);
            int getIntValue(const std::string& key);
            unsigned int getUintValue(const std::string& key);
            double getDoubleValue(const std::string& key);

            //NOTE: the pointer returned from getStringValue must be free'd
            //the application is responsible for releasing this memory, not spino
            const char* getStringValue(const std::string& key);

            bool hasKey(const std::string& key);

            //std::string runScript(const std::string& script);

        private:
            static std::string make_reply(bool success, const std::string& msg) {
                std::stringstream ss;
                if(success) {
                    ss << "{\"msg\":\"" << msg << "\"}";
                }
                else {
                    cout << "Command error: " << msg << endl;
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
            Collection* keyStore = nullptr;
            DocType doc;
            JournalWriter jw;
    };

    std::string escape(const std::string& str);
    std::string unescape(const std::string& str);
}


#endif


