#ifndef SPINO_CURSOR_H
#define SPINO_CURSOR_H

#include "query_parser.h"
#include "query_executor.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "spino_squirrel.h"

#include <iostream>
using namespace std;


namespace Spino
{
    class Cursor
    {
    public:
        Cursor(std::vector<Index>* indices, const std::string &query) : indices(indices), query(query), query_parser(*indices, query)
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

        const char *next()
        {
            const DomView *result = next_dom();
            sb.Clear();
            rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
            if (result != nullptr)
            {
                result->stringify(writer);
                return sb.GetString();
            }
            else 
            {
                return nullptr;
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

        std::string run_script(std::string script) {
            HSQUIRRELVM v;
            v = sq_open(1024); 
            sq_setprintfunc(v, squirrelPrintFunc, squirrelErrorFunc);
            sq_setcompilererrorhandler(v, squirrelCompileError);

            loadSquirrelStdLib(v);

            sq_pushroottable(v);
            sq_compilebuffer(
                    v, script.c_str(), script.length(), "cursor script", true);
            sq_pushroottable(v);
            sq_call(v, 1, SQFalse, SQFalse);


            while(has_next()) {
                sq_pushroottable(v);
                sq_pushstring(v,"result",-1);
                sq_get(v,-2); //get the function from the root table
                sq_pushroottable(v); //'this' (function environment object)
                squirrelPushJsonObj(v, next_dom());
                sq_call(v,2,SQFalse,SQFalse);
                sq_pop(v,2); //pops the roottable and the function
            }

            sq_pushroottable(v);
            sq_pushstring(v, "stringify", -1);
            sq_get(v, -2);
            sq_pushstring(v, "finalize", -1);
            sq_get(v, -3);
            sq_pushroottable(v);
            sq_call(v, 1, SQTrue, SQFalse);

            if(sq_gettype(v, -1) != OT_STRING) {
                sq_call(v, 2, SQTrue, SQFalse);
            }

            std::string ret;
            const char* pcstr;
            sq_getstring(v, -1, &pcstr);
            ret = pcstr;
            sq_pop(v, 2);
            sq_close(v);
            return ret;
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
        rapidjson::StringBuffer sb;
    };
}

#endif
