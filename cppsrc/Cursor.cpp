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
    BaseCursor::BaseCursor() {
        max_results = UINT32_MAX;
        projection_set = false;
    }

    BaseCursor* BaseCursor::setProjection(const char* p) {
        projection.Parse(p);
        projection_set = true;
        return this;
    }

    BaseCursor* BaseCursor::setLimit(uint32_t limit) {
        max_results = limit;
        return this;
    }


    void BaseCursor::apply_projection(
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

    LinearCursor::LinearCursor(ValueType& list, const char* query) : list(list) { 
        Spino::QueryParser parser(query);
        try {
            head = parser.parse_expression();
        }
        catch(parse_error& e) {
            cout << "SpinoDB:: parse error: " << e.what() << endl;
            counter = max_results;
        }
        iter = list.Begin();
        findNext();
    }

    LinearCursor::~LinearCursor() { }

    bool LinearCursor::hasNext() {
        return has_next;
    }

    std::string LinearCursor::next() {
        if(has_next) {
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            if(projection_set) {
                apply_projection(projection, *iter, writer);
            }
            else {
                iter->Accept(writer);
            }
            std::string ret = buffer.GetString();

            iter++;
            findNext();
            return ret;
        }
        return "";

    }

    uint32_t LinearCursor::count() {
        uint32_t r = 0;
        auto itr = list.Begin();
        while(itr != list.End()) {
            exec.set_json(&(*itr));
            if(exec.resolve(head)) {
                r++;
            }
            itr++;
        }

        return r;
    }

    void LinearCursor::findNext() {
        has_next = false;
        if(counter < max_results) {
            while(iter != list.End()) {
                exec.set_json(&(*iter));	
                if(exec.resolve(head)) {
                    has_next = true;
                    counter++;
                    return;
                }
                else {
                    iter++;
                }
            }               
        }
    }


    std::string LinearCursor::runScript(std::string script) {
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


        while(has_next) {
            sq_pushroottable(v);
            sq_pushstring(v,"result",-1);
            sq_get(v,-2); //get the function from the root table
            sq_pushroottable(v); //'this' (function environment object)
            squirrelPushJsonObj(v, *iter);
            sq_call(v,2,SQFalse,SQFalse);
            sq_pop(v,2); //pops the roottable and the function

            iter++;
            findNext();
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


    IndexCursor::IndexCursor(IndexIteratorRange iter_range, ValueType& collection_dom) : 
        collection_dom(collection_dom),
        iter_range(iter_range)
    {
        iter = iter_range.first;
        next();
    }

    IndexCursor::~IndexCursor() { }

    bool IndexCursor::hasNext() {
        if((counter < max_results) && (iter != iter_range.second)) {
            return true;
        }
        else {
            return false;
        }
    }

    std::string IndexCursor::next() {
        if(counter < max_results) {
            if(iter != iter_range.second) {
                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                if(projection_set) {
                    apply_projection(projection, 
                            collection_dom[iter->second], writer);
                }
                else {
                    collection_dom[iter->second].Accept(writer);
                }
                iter++;
                counter++;
                return buffer.GetString();
            } 
        }
        return "";
    }

    uint32_t IndexCursor::count() {
        uint32_t r = 0;
        auto itr = iter_range.first;
        while(itr != iter_range.second) {
            itr++;
            r++;
        }	
        return r;
    }

    std::string IndexCursor::runScript(std::string script) {
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
            squirrelPushJsonObj(v, collection_dom[iter->second]);
            sq_call(v,2,SQFalse,SQFalse);
            sq_pop(v,2); //pops the roottable and the function

            iter++;
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
}

