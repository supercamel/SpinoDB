//  Copyright 2022 Sam Cowen <samuel.cowen@camelsoftware.com>
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


#ifndef SPINO_COLLECTION_H
#define SPINO_COLLECTION_H

#include "Cursor.h"
#include "Journal.h"

namespace Spino
{

    class Collection {
        public:
            Collection(DocType& doc, JournalWriter& jw, std::string name);
            ~Collection();

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
            JournalWriter& jw;
            std::map<uint32_t, std::string> hashmap;

            const uint32_t FNV_PRIME = 16777619u;
            const uint32_t OFFSET_BASIS = 2166136261u;
            uint64_t last_append_timestamp = 0;
    };

}

#endif

