#ifndef SPINO_DATABASE_H
#define SPINO_DATABASE_H

#include <fstream>
#include "collection.h"

namespace Spino
{

    enum FILE_ERROR
    {
        FILE_ERROR_NONE,
        FILE_ERROR_PARSE_ERROR,
        FILE_ERROR_DOES_NOT_EXIST,
        FILE_ERROR_WRITE_ERROR,
        FILE_ERROR_INVALID
    };

    class Database
    {
        public:
            Database();
            ~Database();
            Collection *get_collection(const std::string& name);
            bool has_collection(const std::string& name) const;
            void drop_collection(const std::string& name);
            void set_bool_value(const std::string& key, bool value);
            bool get_bool_value(const std::string& key) const;
            void set_int_value(const std::string& key, int value);
            int get_int_value(const std::string& key) const;
            void set_uint_value(const std::string& key, unsigned int value);
            unsigned int get_uint_value(const std::string& key) const;
            void set_double_value(const std::string& key, double value);
            double get_double_value(const std::string& key) const;
            void set_string_value(const std::string& key, const std::string& value);
            const char* get_string_value(const std::string& key) const;
            bool has_key(const std::string& key) const;
            FILE_ERROR save(const std::string& path) const;
            FILE_ERROR load(const std::string& path);
            void save_not_bson_collection(const std::string& col_name, const std::string& path) const;
            void save_not_bson_keystore(const std::string& path) const;
            void load_not_bson_collection(const std::string& col_name, const std::string& path);
            void save_json(const std::string& path) const;
            void enable_journal(const std::string& jpath);
            void disable_journal();
            void consolidate(const std::string& path);
            std::string execute(const std::string& json);

            void print_keystore()
            {
                kvstore->print();
            }

        private:
            void save_json_collection(ofstream &ofile, Collection* col) const;


            Collection* kvstore;
            std::vector<Collection*> collections;
            JournalWriter jw;
    };
}

#endif
