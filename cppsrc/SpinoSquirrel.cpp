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


#include "SpinoSquirrel.h"
#include "sqstdblob.h"
#include "sqstdio.h"
#include "sqstdmath.h"
#include "sqstdstring.h"
#include "sqstdsystem.h"
#include <stdarg.h>
#include <iostream>
using namespace std;

static const char* stringify_script = R"(

function stringify(v) {
    switch (typeof v) {
        case "table":
            local s = "{";
            local count = 0;
            foreach(idx,var in v) {
                if(count > 0) {
                    s += ",";
                }
                count += 1;
                s += "\"" 
                s += escape(idx) 
                s += "\":" 
                s += stringify(var);
            }
            s += "}";
            return s;
        case "array":
            local s = "[";
            if(v.len() > 0) {
                s += stringify(v[0]);
                for(local i = 1; i < v.len(); i++) {
                    s += ",";
                    s += stringify(v[i]);
                }
            }
            s += "]";
            return s;
        case "string":
            local r = "\"";
            r += escape(v);
            r += "\"";
            return r; 
        case "null":
            return "null";
        default:
            return v.tostring();
    }
}

)";

namespace Spino {
    void squirrelPushJsonObj(HSQUIRRELVM vm, const ValueType& jsonObj) {
        sq_newtable(vm);

        for (ValueType::ConstMemberIterator itr = jsonObj.MemberBegin();
                itr != jsonObj.MemberEnd(); ++itr)
        {
            sq_pushstring(vm, itr->name.GetString(), -1);

            if(itr->value.IsString()) {
                sq_pushstring(vm, itr->value.GetString(), -1);
                sq_newslot(vm, -3, false);
            }
            else if(itr->value.IsBool()) {
                sq_pushbool(vm, itr->value.GetBool());
                sq_newslot(vm, -3, false);
            }
            else if(itr->value.IsNumber()) {
                if(itr->value.IsInt()) {
                    sq_pushinteger(vm, itr->value.GetInt());
                    sq_newslot(vm, -3, false);
                }
                else {
                    sq_pushfloat(vm, itr->value.GetDouble());
                    sq_newslot(vm, -3, false);
                }
            }
            else if(itr->value.IsArray()) { 
                squirrelPushJsonArr(vm, itr->value);
                sq_newslot(vm, -3, false);
            }
            else if(itr->value.IsObject()) { 
                squirrelPushJsonObj(vm, itr->value);
                sq_newslot(vm, -3, false);
            }
            else if(itr->value.IsNull()) {
                sq_pushnull(vm);
                sq_newslot(vm, -3, false);
            }
            else {
                sq_pop(vm, 1);
            }
        }
    }

    void squirrelPushJsonArr(HSQUIRRELVM vm, const ValueType& jsonArr) {
        sq_newarray(vm, 0);

        int count = 0;
        for (ValueType::ConstValueIterator itr = jsonArr.Begin();
                itr != jsonArr.End(); ++itr)
        {
            if(itr->IsString()) {
                sq_pushstring(vm, itr->GetString(), -1);
                sq_arrayappend(vm, -2);
            }
            else if(itr->IsBool()) {
                sq_pushbool(vm, itr->GetBool());
                sq_arrayappend(vm, -2);
            }
            else if(itr->IsNumber()) {
                if(itr->IsInt()) {
                    sq_pushinteger(vm, itr->GetInt());
                    sq_arrayappend(vm, -2);
                }
                else {
                    sq_pushfloat(vm, itr->GetDouble());
                    sq_arrayappend(vm, -2);
                }
            }
            else if(itr->IsArray()) { 
                squirrelPushJsonArr(vm, itr->GetArray());
                sq_arrayappend(vm, -2);
            }
            else if(itr->IsObject()) { 
                squirrelPushJsonObj(vm, itr->GetObject());
                sq_arrayappend(vm, -2);
            }
            
            else if(itr->IsNull()) {
                sq_pushnull(vm);
                sq_newslot(vm, -2, false);
            }
            else {
                sq_pop(vm, 1);
            }
        }
    }

    void loadSquirrelStdLib(HSQUIRRELVM vm) {
        sq_pushroottable(vm);
        sqstd_register_stringlib(vm);
        sqstd_register_bloblib(vm);
        sqstd_register_iolib(vm);
        sqstd_register_mathlib(vm);
        sqstd_register_systemlib(vm);
        sq_pop(vm, 1);

        sq_pushroottable(vm);
        sq_compilebuffer(
                    vm, stringify_script, strlen(stringify_script), "stringify script", true);
        sq_pushroottable(vm);
        sq_call(vm, 1, SQFalse, SQFalse);
        sq_pop(vm, 1);
    }


    void squirrelPrintFunc(HSQUIRRELVM vm, const SQChar* s, ...) {
        char buf[256]; 
        va_list args;
        va_start (args, s);
        vsnprintf(buf, 256, s, args);
        va_end (args);
        cout << "Squirrel:: " << buf << endl;
    }

    void squirrelErrorFunc(HSQUIRRELVM vm, const SQChar* s, ...) {
        char buf[256]; 
        va_list args;
        va_start (args, s);
        vsnprintf(buf, 256, s, args);
        va_end (args);
        cout << "SquirrelError:: " << buf << endl;
    }

    void squirrelCompileError(HSQUIRRELVM vm, const SQChar* desc,
            const SQChar* source, SQInteger line, SQInteger column) {
        cout << "Squirrel Compile Error: " << desc << endl;
        cout << "Line: " << line << " Col: " << column << endl;
    }

}

