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

namespace Spino {

    class SpinoDB {
        public:
            SpinoDB() {
                doc.SetObject();

                const char* keystoreName = "__SpinoKeyValueStore__";
                ValueType v(rapidjson::kArrayType);
                ValueType index(keystoreName, doc.GetAllocator());
                doc.AddMember(index, v, doc.GetAllocator());

                keyStore = new Collection(doc, keystoreName);
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

            void set_path();

            void save(const std::string& path) const;
            void load(const std::string& path);

            void setIntValue(const std::string& key, int value);
            void setUintValue(const std::string& key, unsigned int value);
            void setDoubleValue(const std::string& key, double value);
            void setStringValue(const std::string& key, const std::string& value);

            int getIntValue(const std::string& key);
            unsigned int getUintValue(const std::string& key);
            double getDoubleValue(const std::string& key);
            std::string getStringValue(const std::string& key);

            bool hasKey(const std::string& key);

        private:
            static std::string make_reply(bool success, const std::string& msg) {
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
            Collection* keyStore = nullptr;
            DocType doc;

            std::string path;
    };

    std::string escape(const std::string& str);

}


#endif


