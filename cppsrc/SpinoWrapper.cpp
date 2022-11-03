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


#include "SpinoWrapper.h"

#include <regex>

#include <iostream>
using namespace std;


using node::AddEnvironmentCleanupHook;
using v8::Exception;
using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::ObjectTemplate;
using v8::String;
using v8::Value;
using v8::Array;

v8::Global<Function> CursorWrapper::constructor;
v8::Global<Function> CollectionWrapper::constructor;


void CursorWrapper::Init(Isolate* isolate){
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate);
    tpl->SetClassName(String::NewFromUtf8(isolate, "Cursor").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "hasNext", hasNext);
    NODE_SET_PROTOTYPE_METHOD(tpl, "next", next);
    NODE_SET_PROTOTYPE_METHOD(tpl, "toArray", toArray);
    NODE_SET_PROTOTYPE_METHOD(tpl, "count", count);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setLimit", setLimit);

    Local<Context> context = isolate->GetCurrentContext();
    constructor.Reset(isolate, tpl->GetFunction(context).ToLocalChecked());

    AddEnvironmentCleanupHook(isolate, [](void*) {
            constructor.Reset();
            }, nullptr);

}

void CursorWrapper::NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args, shared_ptr<Spino::Cursor> cur) {
    Isolate* isolate = args.GetIsolate();

    auto cons = Local<Function>::New(isolate, constructor);
    auto context = isolate->GetCurrentContext();
    auto instance = cons->NewInstance(context).ToLocalChecked();
    auto curwrapper = new CursorWrapper(cur);
    curwrapper->Wrap(instance);
    args.GetReturnValue().Set(instance);
}

void CursorWrapper::toArray(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CursorWrapper* curwrap = ObjectWrap::Unwrap<CursorWrapper>(args.Holder());
    Local<Array> nodes = Array::New(isolate);

    auto txt = curwrap->cursor->next();
    uint32_t count = 0;
    while(txt != "") {
        auto v8str = v8::String::NewFromUtf8(isolate, txt.c_str());
        if(!v8str.IsEmpty()) {
            auto v8strlocal = v8str.ToLocalChecked();
            auto jsonobj = v8::JSON::Parse(isolate->GetCurrentContext(), v8strlocal);
            if(!jsonobj.IsEmpty()) {
                nodes->Set(isolate->GetCurrentContext(), count++, jsonobj.ToLocalChecked());
            }
        }	
        else {
            return;
        }

        txt = curwrap->cursor->next();
    }

    args.GetReturnValue().Set(nodes);
}

void CursorWrapper::hasNext(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CursorWrapper* curwrap = ObjectWrap::Unwrap<CursorWrapper>(args.Holder());
    bool hasnext = curwrap->cursor->has_next();
    args.GetReturnValue().Set(v8::Boolean::New(isolate, hasnext));
}

void CursorWrapper::next(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CursorWrapper* curwrap = ObjectWrap::Unwrap<CursorWrapper>(args.Holder());
    auto next = curwrap->cursor->next();
    if(next != "") {
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, next.c_str()).ToLocalChecked());
    }
}

void CursorWrapper::count(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CursorWrapper* curwrap = ObjectWrap::Unwrap<CursorWrapper>(args.Holder());

    args.GetReturnValue().Set(v8::Number::New(isolate, curwrap->cursor->count()));
}

void CursorWrapper::setLimit(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CursorWrapper* curwrap = ObjectWrap::Unwrap<CursorWrapper>(args.Holder());

    if(args[0]->IsNumber()) {
        uint32_t limit = args[0].As<Number>()->Value();
        curwrap->cursor->set_limit(limit);
    }
    else {
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        "Expected parameter for setLimit to be a number.").ToLocalChecked()));

    }

    args.GetReturnValue().Set(args.Holder());
}

void CollectionWrapper::Init(Isolate* isolate){
    // Prepare constructor template
    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate);
    tpl->SetClassName(String::NewFromUtf8(isolate, "Collection").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    // Prototype
    NODE_SET_PROTOTYPE_METHOD(tpl, "getName", getName);
    NODE_SET_PROTOTYPE_METHOD(tpl, "createIndex", createIndex);
    NODE_SET_PROTOTYPE_METHOD(tpl, "dropIndex", dropIndex);
    NODE_SET_PROTOTYPE_METHOD(tpl, "append", append);
    NODE_SET_PROTOTYPE_METHOD(tpl, "upsert", upsert);
    NODE_SET_PROTOTYPE_METHOD(tpl, "findOne", findOne);
    NODE_SET_PROTOTYPE_METHOD(tpl, "find", find);
    NODE_SET_PROTOTYPE_METHOD(tpl, "dropOne", dropOne);
    NODE_SET_PROTOTYPE_METHOD(tpl, "drop", drop);

    Local<Context> context = isolate->GetCurrentContext();
    constructor.Reset(isolate, tpl->GetFunction(context).ToLocalChecked());

    AddEnvironmentCleanupHook(isolate, [](void*) {
            constructor.Reset();
            }, nullptr);

}

void CollectionWrapper::NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args, Spino::Collection* col) {
    Isolate* isolate = args.GetIsolate();

    auto cons = Local<Function>::New(isolate, constructor);
    auto context = isolate->GetCurrentContext();
    auto instance = cons->NewInstance(context).ToLocalChecked();
    auto colwrapper = new CollectionWrapper(col);
    colwrapper->Wrap(instance);
    args.GetReturnValue().Set(instance);
}

void CollectionWrapper::getName(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CollectionWrapper* collectionwrap = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());
    auto name = collectionwrap->collection->get_name();
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked());
}

void CollectionWrapper::createIndex(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    obj->collection->create_index(*str);
}

void CollectionWrapper::dropIndex(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    obj->collection->drop_index(*str);
}

void CollectionWrapper::append(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    if(args[0]->IsString()) {
        v8::String::Utf8Value str(isolate, args[0]);
        obj->collection->append(*str);
    } 
    else if(args[0]->IsObject()) {
        auto handle = args[0].As<v8::Object>();
        auto jsonobj = v8::JSON::Stringify(isolate->GetCurrentContext(), handle).ToLocalChecked();
        v8::String::Utf8Value s(isolate, jsonobj);
        obj->collection->append(*s);
    }
}

void CollectionWrapper::upsert(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value findstr(isolate, args[0]);
    v8::String::Utf8Value update(isolate, args[1]);

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    try {
        obj->collection->upsert(*findstr, *update);
    }
    catch(Spino::parse_error& err){
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        err.what()).ToLocalChecked()));
    }
    catch(std::regex_error& err) {
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        err.what()).ToLocalChecked()));
    }
    catch(...) {
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        "Unknown exception caught during update query").ToLocalChecked()));
    }

}

void CollectionWrapper::findOne(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    std::string f;
    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());
    try {
        Spino::DomView* view = obj->collection->find_one(*str);
        if(view != nullptr) {
            f = view->stringify();
        }
        else {
            f = "";
        }
    }
    catch(Spino::parse_error& err){
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        err.what()).ToLocalChecked()));
    }
    catch(std::regex_error& err) {
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        err.what()).ToLocalChecked()));
    }
    catch(...) {
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        "Unknown exception caught during find query").ToLocalChecked()));
    }
    if(f != "") {
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, f.c_str()).ToLocalChecked());
    }
}

void CollectionWrapper::find(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);


    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());
    try {
        auto cursor = obj->collection->find(*str);
        if(args.Length() >= 2) {
            uint32_t limit = args[1].As<Number>()->Value();
            cursor = cursor->set_limit(limit);
        }
        CursorWrapper::NewInstance(args, cursor);
    }
    catch(Spino::parse_error& err){
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        err.what()).ToLocalChecked()));
    }
    catch(std::regex_error& err) {
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        err.what()).ToLocalChecked()));
    }
    catch(...) {
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        "Unknown exception caught during find query").ToLocalChecked()));
    }

}

void CollectionWrapper::dropOne(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value findstr(isolate, args[0]);

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    obj->collection->drop(*findstr, 1);
}

void CollectionWrapper::drop(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value findstr(isolate, args[0]);
    uint32_t limit = UINT32_MAX;
    if(args.Length() >= 2) {
        limit = args[1].As<Number>()->Value();
    }

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    auto n_dropped = obj->collection->drop(*findstr, limit);
    args.GetReturnValue().Set(v8::Number::New(isolate, n_dropped));
}

SpinoWrapper::SpinoWrapper() {
    spino = new Spino::Database();	
}

void SpinoWrapper::Init(Local<Object> exports) {
    Isolate* isolate = exports->GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    Local<ObjectTemplate> addon_data_tpl = ObjectTemplate::New(isolate);
    addon_data_tpl->SetInternalFieldCount(1);  
    Local<Object> addon_data = addon_data_tpl->NewInstance(context).ToLocalChecked();

    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate, New, addon_data);
    tpl->SetClassName(String::NewFromUtf8(isolate, "Spino").ToLocalChecked());
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    NODE_SET_PROTOTYPE_METHOD(tpl, "execute", execute);
    NODE_SET_PROTOTYPE_METHOD(tpl, "save", save);
    NODE_SET_PROTOTYPE_METHOD(tpl, "load", load);
    NODE_SET_PROTOTYPE_METHOD(tpl, "enableJournal", enableJournal);
    NODE_SET_PROTOTYPE_METHOD(tpl, "disableJournal", disableJournal);
    NODE_SET_PROTOTYPE_METHOD(tpl, "consolidate", consolidate);

    NODE_SET_PROTOTYPE_METHOD(tpl, "getCollection", getCollection);
    NODE_SET_PROTOTYPE_METHOD(tpl, "dropCollection", dropCollection);

    NODE_SET_PROTOTYPE_METHOD(tpl, "setBoolValue", setBoolValue);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setIntValue", setIntValue);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setUintValue", setUintValue);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setDoubleValue", setDoubleValue);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setStringValue", setStringValue);

    NODE_SET_PROTOTYPE_METHOD(tpl, "getBoolValue", getBoolValue);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getIntValue", getIntValue);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getUintValue", getUintValue);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getDoubleValue", getDoubleValue);
    NODE_SET_PROTOTYPE_METHOD(tpl, "getStringValue", getStringValue);

    NODE_SET_PROTOTYPE_METHOD(tpl, "hasKey", hasKey);

    Local<Function> constructor = tpl->GetFunction(context).ToLocalChecked();
    addon_data->SetInternalField(0, constructor);
    exports->Set(context, String::NewFromUtf8(
                isolate, "Spino").ToLocalChecked(),
            constructor).FromJust();
}

void SpinoWrapper::New(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Local<Context> context = isolate->GetCurrentContext();

    if (args.IsConstructCall()) {
        SpinoWrapper* obj = new SpinoWrapper();
        obj->Wrap(args.This());
        args.GetReturnValue().Set(args.This());
    } else {
        Local<Function> cons =
            args.Data().As<Object>()->GetInternalField(0).As<Function>();
        Local<Object> result =
            cons->NewInstance(context).ToLocalChecked();
        args.GetReturnValue().Set(result);
    }
}

void SpinoWrapper::execute(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    if(args[0]->IsString()) {
        v8::String::Utf8Value str(isolate, args[0]);

        auto response = obj->spino->execute(*str);
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, response.c_str()).ToLocalChecked());
    } 
    else if(args[0]->IsObject()) {
        auto handle = args[0].As<v8::Object>();

        auto jsonobj = v8::JSON::Stringify(isolate->GetCurrentContext(), handle).ToLocalChecked();
        v8::String::Utf8Value str(isolate, jsonobj);
        std::string response = "";
        try {
            response = obj->spino->execute(*str);
        }
        catch(Spino::parse_error& err) {
            response = "Query parse error: ";
            response += err.what();
        }
        catch(std::regex_error& err) {
            response = "Bad regex: ";
            response += err.what();
        }
        catch(...) {
            response = "Unknown exception caught during query";
        }

        if(response != "") {
            auto v8str = v8::String::NewFromUtf8(isolate, response.c_str());
            if(!v8str.IsEmpty()) {
                auto v8strlocal = v8str.ToLocalChecked();
                auto jsonobj = v8::JSON::Parse(isolate->GetCurrentContext(), v8strlocal);
                if(!jsonobj.IsEmpty()) {
                    args.GetReturnValue().Set(jsonobj.ToLocalChecked());
                }
            }
        }
    }
}

void SpinoWrapper::save(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->save(*str);
}

void SpinoWrapper::load(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->load(*str);
}

void SpinoWrapper::enableJournal(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->enable_journal(*str);
}

void SpinoWrapper::disableJournal(const FunctionCallbackInfo<Value>& args) {
    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    obj->spino->disable_journal();
}

void SpinoWrapper::consolidate(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->consolidate(*str);
}

void SpinoWrapper::getCollection(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    SpinoWrapper* spinowrap = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    auto col = spinowrap->spino->get_collection(*str);
    if(col == nullptr) {
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        "Collection doesn't exist").ToLocalChecked()));

    }
    else {
        CollectionWrapper::NewInstance(args, col);
    }
}


void SpinoWrapper::dropCollection(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->drop_collection(*str);
}

void SpinoWrapper::setBoolValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->set_bool_value(*key, args[1].As<v8::Boolean>()->Value());
}

void SpinoWrapper::setIntValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->set_int_value(*key, args[1].As<Number>()->Value());
}

void SpinoWrapper::setUintValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->set_uint_value(*key, args[1].As<Number>()->Value());
}

void SpinoWrapper::setDoubleValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->set_double_value(*key, args[1].As<Number>()->Value());
}

void SpinoWrapper::setStringValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);
    v8::String::Utf8Value value(isolate, args[1]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->set_string_value(*key, *value);
}

void SpinoWrapper::getBoolValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    bool v = obj->spino->get_bool_value(*key);
    args.GetReturnValue().Set(v8::Boolean::New(isolate, v));
}

void SpinoWrapper::getIntValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    int v = obj->spino->get_int_value(*key);
    args.GetReturnValue().Set(v8::Number::New(isolate, v));
}

void SpinoWrapper::getUintValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    unsigned int v = obj->spino->get_uint_value(*key);
    args.GetReturnValue().Set(v8::Number::New(isolate, v));
}

void SpinoWrapper::getDoubleValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    double v = obj->spino->get_double_value(*key);
    args.GetReturnValue().Set(v8::Number::New(isolate, v));
}

void SpinoWrapper::getStringValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    const char* v = obj->spino->get_string_value(*key);
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, v).ToLocalChecked());
    delete v;
}

void SpinoWrapper::hasKey(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    bool v = obj->spino->has_key(*key);
    args.GetReturnValue().Set(v8::Boolean::New(isolate, v));
}

