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


#include <node.h>
#include "SpinoWrapper.h"

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::String;
using v8::Value;

namespace SpinoDB {

    using v8::Local;
    using v8::Object;

    void escape(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        v8::String::Utf8Value str(isolate, args[0]);

        std::string escaped = Spino::escape(*str);
        auto v8str = v8::String::NewFromUtf8(isolate, escaped.c_str());
        if(!v8str.IsEmpty()) {
            args.GetReturnValue().Set(v8str.ToLocalChecked());
        }
    }

    void InitAll(Local<Object> exports) {
        CursorWrapper::Init(exports->GetIsolate());
        CollectionWrapper::Init(exports->GetIsolate());
        SpinoWrapper::Init(exports);
        NODE_SET_METHOD(exports, "escape", escape);
    }

    NODE_MODULE(NODE_GYP_MODULE_NAME, InitAll)

}
