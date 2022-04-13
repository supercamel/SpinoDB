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


#include "Cursor.h"
#include "SpinoSquirrel.h"

#include <iostream>
using namespace std;


namespace Spino {
    Cursor::Cursor(
            shared_ptr<QueryNode> qn, 
            shared_ptr<IndexIteratorRange> iter_range, 
            ValueType& collection_dom) : 
        collection_dom(collection_dom),
        iter_range(iter_range)
    {
        max_results = UINT32_MAX;
        projection_set = false;
        head = qn;
        iter = iter_range->first;

        if(iter != iter_range->second) {
            exec.set_json(&collection_dom[iter->second]);
            while(exec.resolve(head) == false) {
                iter++;
                if(iter == iter_range->second) {
                    break;
                }
                else {
                    exec.set_json(&collection_dom[iter->second]);
                }
            }
        }
    }

    Cursor::~Cursor() { }


    Cursor* Cursor::setProjection(const char* p) {
        projection.Parse(p);
        projection_set = true;
        return this;
    }

    Cursor* Cursor::setLimit(size_t limit) {
        max_results = limit;
        return this;
    }


    void Cursor::apply_projection(
            const ValueType& proj, 
            const ValueType& source, 
            rapidjson::Writer<rapidjson::StringBuffer>& writer) 
    {
        writer.StartObject();

        // white list projection only
        for (ValueType::ConstMemberIterator itr = proj.MemberBegin();
                itr != proj.MemberEnd(); ++itr)
        {
            std::string name = itr->name.GetString();

            auto srcIt = source.GetObject().FindMember(name.c_str()); 
            // if the source object has the projection field
            if(srcIt != source.MemberEnd()) {
                writer.Key(name.c_str());
                if(srcIt->value.IsObject() && itr->value.IsObject()) {
                    apply_projection(
                            itr->value.GetObject(), srcIt->value, writer);

                }
                else {
                    srcIt->value.Accept(writer);
                }
            }
        }

        writer.EndObject();
    }


    std::string Cursor::runScript(std::string script) {
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


        while(hasNext()) {
            sq_pushroottable(v);
            sq_pushstring(v,"result",-1);
            sq_get(v,-2); //get the function from the root table
            sq_pushroottable(v); //'this' (function environment object)
            squirrelPushJsonObj(v, *nextAsJsonObj());
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



    bool Cursor::hasNext() {
        if((counter < max_results) && (iter != iter_range->second)) {
            return true;
        }
        else {
            return false;
        }
    }

    std::string Cursor::next() {
        if(counter < max_results) {
            if(iter != iter_range->second) {
                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                if(projection_set) {
                    apply_projection(projection, 
                            collection_dom[iter->second], writer);
                }
                else {
                    collection_dom[iter->second].Accept(writer);
                }


                counter++;
                if(counter != max_results) {
                    while(++iter != iter_range->second) {
                        exec.set_json(&collection_dom[iter->second]);
                        if(exec.resolve(head)) {
                            break;
                        }
                    }
                }

                return buffer.GetString();
            } 
        }
        return "";
    }

    size_t Cursor::count() {
        size_t r = 0;
        auto itr = iter_range->first;
        while(itr != iter_range->second) {
            exec.set_json(&collection_dom[itr->second]);
            if(exec.resolve(head)) {
                r++;
            }
            itr++;
        }	
        return r;
    }

    const ValueType* Cursor::nextAsJsonObj() {
        if(counter < max_results) {
            if(iter != iter_range->second) {
                const ValueType& ret = collection_dom[iter->second];


                counter++;
                if(counter != max_results) {
                    while(++iter != iter_range->second) {
                        exec.set_json(&collection_dom[iter->second]);
                        if(exec.resolve(head)) {
                            break;
                        }
                    }
                }

                return &ret;
            } 
        }
        return nullptr;
    }


}

