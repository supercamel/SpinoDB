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
    struct CompoundIndexCmp {
        bool operator()(const std::vector<Spino::Value>& lhs, 
                const std::vector<Spino::Value>& rhs) {
            size_t lh_len = lhs.size();
            size_t rh_len = rhs.size();
            if(lh_len != rh_len) {
                return false;
            }

            for(size_t i = 0; i < lh_len; i++) {
                if(lhs > rhs) {
                    return false;
                }
            }

            return true;
        }
    };

    class Index {
        public:
            std::string field_name;
            PointerType field;
            std::multimap<Spino::Value, size_t> index;
    };

    class CompoundIndex {
        public:
            std::vector<std::string> field_names;
            std::vector<PointerType> fields;
            std::multimap<std::vector<Spino::Value>, size_t, CompoundIndexCmp> index;
    };


    class Collection {
        public:
            Collection(DocType& doc, JournalWriter& jw, std::string name);
            ~Collection();

            std::string getName() const;

            void createIndex(const char* field);
            void createCompoundIndex(const char** field_names, size_t len);
            void dropIndex(const char* field);

            void append(ValueType& d);
            void append(const char* s);

            void updateById(const char* id, const char* update);
            void update(const char* search, const char* update);

            std::string findOneById(const char* id) const;
            std::string findOne(const char* s);
            Cursor* find(const char* s) const;

            void dropById(const char* s);
            void dropOne(const char* s);
            size_t drop(const char* s, size_t limit = UINT32_MAX);
            size_t dropOlderThan(uint64_t timestamp); //milliseconds since 1970 epoch

            static uint64_t timestampById(const char* id);

            size_t size() {
                auto& arr = doc[name.c_str()];
                return arr.Size();
            }

        private:

            void indexNewDoc();
            void removeDomIdxFromIndex(size_t domIdx);
            bool domIndexFromId(const char* s, size_t& domIdx) const;
            void reconstructIndices();

            std::vector<Index*> indices;
            std::vector<CompoundIndex*> compound_indices;
            bool mergeObjects(ValueType& dstObject, ValueType& srcObject);

            size_t fnv1a_hash(std::string& s);
            static uint64_t fast_atoi_len(const char * str, size_t len)
            {
                uint64_t val = 0;
                size_t count = 0;
                while(count++ < len) {
                    val = val*10 + (*str++ - '0');
                }
                return val;
            }

            size_t id_counter;
            std::string name;
            DocType& doc;
            JournalWriter& jw;
            std::map<size_t, std::string> hashmap;

            const uint32_t FNV_PRIME = 16777619u;
            const uint32_t OFFSET_BASIS = 2166136261u;
            uint64_t last_append_timestamp = 0;
    };

}

#endif

