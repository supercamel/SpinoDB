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


#include "spino_squirrel.h"
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
    void squirrelPushJsonObj(HSQUIRRELVM vm, const DomView* jsonObj) {
        sq_newtable(vm);

        for (auto itr = jsonObj->member_begin();
                itr != jsonObj->member_end(); ++itr)
        {
            sq_pushstring(vm, itr.get_key().c_str(), -1);

            if(itr.get_value().is_string()) {
                sq_pushstring(vm, itr.get_value().get_string(), -1);
                sq_newslot(vm, -3, false);
            }
            else if(itr.get_value().get_type() == DOM_NODE_TYPE_BOOL) {
                sq_pushbool(vm, itr.get_value().get_bool());
                sq_newslot(vm, -3, false);
            }
            else if(itr.get_value().is_numeric()) {
                if(itr.get_value().get_type() == DOM_NODE_TYPE_INT) {
                    sq_pushinteger(vm, itr.get_value().get_int());
                }
                else {
                    sq_pushfloat(vm, itr.get_value().get_double());
                }
                sq_newslot(vm, -3, false);
            }
            else if(itr.get_value().get_type() == DOM_NODE_TYPE_ARRAY) { 
                squirrelPushJsonArr(vm, &itr.get_value());
                sq_newslot(vm, -3, false);
            }
            else if(itr.get_value().get_type() == DOM_NODE_TYPE_OBJECT) { 
                squirrelPushJsonObj(vm, &itr.get_value());
                sq_newslot(vm, -3, false);
            }
            else if(itr.get_value().get_type() == DOM_NODE_TYPE_NULL) {
                sq_pushnull(vm);
                sq_newslot(vm, -3, false);
            }
            else {
                sq_pop(vm, 1);
            }
        }
    }

    void squirrelPushJsonArr(HSQUIRRELVM vm, const DomView* jsonArr) {
        sq_newarray(vm, 0);

        int count = 0;
        for (auto iter = jsonArr->element_begin();
                iter != jsonArr->element_end(); ++iter)
        {
            if(iter->is_string()) {
                sq_pushstring(vm, iter->get_string(), -1);
                sq_arrayappend(vm, -2);
            }
            else if(iter->get_type() == DOM_NODE_TYPE_BOOL) {
                sq_pushbool(vm, iter->get_bool());
                sq_arrayappend(vm, -2);
            }
            else if(iter->is_numeric()) {
                if(iter->get_type() == DOM_NODE_TYPE_INT) {
                    sq_pushinteger(vm, iter->get_int());
                }
                else {
                    sq_pushfloat(vm, iter->get_double());
                }
                sq_arrayappend(vm, -2);
            }
            else if(iter->get_type() == DOM_NODE_TYPE_ARRAY) { 
                squirrelPushJsonArr(vm, &(*iter));
                sq_arrayappend(vm, -2);
            }
            else if(iter->get_type() == DOM_NODE_TYPE_OBJECT) { 
                squirrelPushJsonObj(vm, &(*iter));
                sq_arrayappend(vm, -2);
            }
            else if(iter->get_type() == DOM_NODE_TYPE_NULL) {
                sq_pushnull(vm);
                sq_arrayappend(vm, -2);
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
