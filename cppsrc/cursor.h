#ifndef SPINO_CURSOR_H
#define SPINO_CURSOR_H

#include "query_parser.h"
#include "query_executor.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <iostream>
using namespace std;


namespace Spino
{
    class Cursor
    {
    public:
        Cursor(std::vector<Index>* indices, const std::string &query) : indices(indices), query(query), query_parser(*indices, query.c_str())
        {
            limit = SIZE_MAX;
            n_results = 0;

            instructions = *query_parser.parse_query(range);
            executor.set_instructions(&instructions);

            iter = range.first;
            if (iter != range.second)
            {
                while (executor.execute_query(*iter->second) == false)
                {
                    auto second = iter->second;

                    iter++;
                    second = iter->second;
                    if (iter == range.second)
                    {
                        break;
                    }
                }
            }
        }

        ~Cursor()
        {
        }

        bool has_next()
        {
            if ((n_results < limit) && (iter != range.second))
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        char *next()
        {
            const DomView *result = next_dom();
            rapidjson::StringBuffer sb;
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            if (result != nullptr)
            {
                result->stringify(writer);
                return strdup(sb.GetString());
            }
            else
            {
                return strdup("");
            }
        }

        DomView *next_dom()
        {
            if (has_next())
            {
                auto second = iter->second;
                DomNode *ret = (*second);

                n_results++;

            
                while (++iter != range.second)
                {
                    if (executor.execute_query(*iter->second))
                    {
                        break;
                    }
                }

                return ret;
            }
            else
            {
                return nullptr;
            }
        }

        size_t count()
        {
            size_t r = 0;
            auto itr = range.first;
            while (itr != range.second)
            {
                if (executor.execute_query(*itr->second))
                {
                    r++;
                }
                itr++;
            }
            return r;
        }

        IndexIterator get_iter()
        {
            return iter;
        }

        void set_limit(size_t l)
        {
            limit = l;
        }

    private:
        std::vector<Index>* indices;
        IndexIterator iter;
        IndexIteratorRange range;
        std::string query;
        std::vector<Token> instructions;
        QueryParser query_parser;
        QueryExecutor executor;
        size_t limit;
        size_t n_results;
    };
}

#endif
