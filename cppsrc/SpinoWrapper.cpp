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
    NODE_SET_PROTOTYPE_METHOD(tpl, "setProjection", setProjection);
    NODE_SET_PROTOTYPE_METHOD(tpl, "setLimit", setLimit);
    NODE_SET_PROTOTYPE_METHOD(tpl, "runScript", runScript);

    Local<Context> context = isolate->GetCurrentContext();
    constructor.Reset(isolate, tpl->GetFunction(context).ToLocalChecked());

    AddEnvironmentCleanupHook(isolate, [](void*) {
            constructor.Reset();
            }, nullptr);

}

void CursorWrapper::NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args, Spino::Cursor* cur) {
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
    bool hasnext = curwrap->cursor->hasNext();
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

void CursorWrapper::setProjection(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CursorWrapper* curwrap = ObjectWrap::Unwrap<CursorWrapper>(args.Holder());

    if(args[0]->IsString()) {
        v8::String::Utf8Value str(isolate, args[0]);

        curwrap->cursor->setProjection(*str);
    }

    args.GetReturnValue().Set(args.Holder());
}

void CursorWrapper::setLimit(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CursorWrapper* curwrap = ObjectWrap::Unwrap<CursorWrapper>(args.Holder());

    if(args[0]->IsNumber()) {
        uint32_t limit = args[0].As<Number>()->Value();
        curwrap->cursor->setLimit(limit);
    }
    else {
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        "Expected parameter for setLimit to be a number.").ToLocalChecked()));

    }

    args.GetReturnValue().Set(args.Holder());
}

void CursorWrapper::runScript(const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CursorWrapper* curwrap = ObjectWrap::Unwrap<CursorWrapper>(args.Holder());
    if(args[0]->IsString()) {
        v8::String::Utf8Value str(isolate, args[0]);

        std::string result = curwrap->cursor->runScript(*str);
        args.GetReturnValue().Set(
                String::NewFromUtf8(isolate, result.c_str()).ToLocalChecked());
    }
    else {
        isolate->ThrowException(Exception::TypeError(
                    String::NewFromUtf8(isolate,
                        "expected parameter for runScript to be a string").ToLocalChecked()));
    }
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
    NODE_SET_PROTOTYPE_METHOD(tpl, "updateById", updateById);
    NODE_SET_PROTOTYPE_METHOD(tpl, "update", update);
    NODE_SET_PROTOTYPE_METHOD(tpl, "findOneById", findOneById);
    NODE_SET_PROTOTYPE_METHOD(tpl, "findOne", findOne);
    NODE_SET_PROTOTYPE_METHOD(tpl, "find", find);
    NODE_SET_PROTOTYPE_METHOD(tpl, "dropById", dropById);
    NODE_SET_PROTOTYPE_METHOD(tpl, "dropOne", dropOne);
    NODE_SET_PROTOTYPE_METHOD(tpl, "drop", drop);
    NODE_SET_PROTOTYPE_METHOD(tpl, "dropOlderThan", dropOlderThan);
    NODE_SET_PROTOTYPE_METHOD(tpl, "timestampById", timestampById);

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
    auto name = collectionwrap->collection->getName();
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, name.c_str()).ToLocalChecked());
}

void CollectionWrapper::createIndex(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    obj->collection->createIndex(*str);
}

void CollectionWrapper::dropIndex(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    obj->collection->dropIndex(*str);
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

void CollectionWrapper::updateById(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value idstr(isolate, args[0]);
    v8::String::Utf8Value update(isolate, args[1]);

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    obj->collection->updateById(*idstr, *update);
}

void CollectionWrapper::update(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value findstr(isolate, args[0]);
    v8::String::Utf8Value update(isolate, args[1]);

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    try {
        obj->collection->update(*findstr, *update);
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

void CollectionWrapper::findOneById(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value idstr(isolate, args[0]);

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());
    auto f = obj->collection->findOneById(*idstr);
    if(f != "") {
        args.GetReturnValue().Set(String::NewFromUtf8(isolate, f.c_str()).ToLocalChecked());
    }
}

void CollectionWrapper::findOne(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    std::string f;
    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());
    try {
        f = obj->collection->findOne(*str);
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
            cursor = cursor->setLimit(limit);
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

void CollectionWrapper::dropById(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value findstr(isolate, args[0]);

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    obj->collection->dropById(*findstr);
}

void CollectionWrapper::dropOne(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value findstr(isolate, args[0]);

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    obj->collection->dropOne(*findstr);
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

void CollectionWrapper::dropOlderThan(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    auto n_dropped = obj->collection->dropOlderThan(args[0].As<Number>()->Value());
    args.GetReturnValue().Set(v8::Number::New(isolate, n_dropped));
}

void CollectionWrapper::timestampById(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value findstr(isolate, args[0]);

    CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

    uint64_t ts = obj->collection->timestampById(*findstr);
    args.GetReturnValue().Set(v8::Date::New(isolate->GetCurrentContext(), ts).ToLocalChecked());
}



SpinoWrapper::SpinoWrapper() {
    spino = new Spino::SpinoDB();	
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

    NODE_SET_PROTOTYPE_METHOD(tpl, "addCollection", addCollection);
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

    obj->spino->enableJournal(*str);
}

void SpinoWrapper::disableJournal(const FunctionCallbackInfo<Value>& args) {
    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    obj->spino->disableJournal();
}

void SpinoWrapper::consolidate(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->consolidate(*str);
}

void SpinoWrapper::addCollection(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    SpinoWrapper* spinowrap = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    auto col = spinowrap->spino->addCollection(*str);
    CollectionWrapper::NewInstance(args, col);
}

void SpinoWrapper::getCollection(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value str(isolate, args[0]);

    SpinoWrapper* spinowrap = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    auto col = spinowrap->spino->getCollection(*str);
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

    obj->spino->dropCollection(*str);
}

void SpinoWrapper::setBoolValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->setBoolValue(*key, args[1].As<v8::Boolean>()->Value());
}

void SpinoWrapper::setIntValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->setIntValue(*key, args[1].As<Number>()->Value());
}

void SpinoWrapper::setUintValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->setUintValue(*key, args[1].As<Number>()->Value());
}

void SpinoWrapper::setDoubleValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->setDoubleValue(*key, args[1].As<Number>()->Value());
}

void SpinoWrapper::setStringValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);
    v8::String::Utf8Value value(isolate, args[1]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

    obj->spino->setStringValue(*key, *value);
}

void SpinoWrapper::getBoolValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    bool v = obj->spino->getBoolValue(*key);
    args.GetReturnValue().Set(v8::Boolean::New(isolate, v));
}

void SpinoWrapper::getIntValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    int v = obj->spino->getIntValue(*key);
    args.GetReturnValue().Set(v8::Number::New(isolate, v));
}

void SpinoWrapper::getUintValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    unsigned int v = obj->spino->getUintValue(*key);
    args.GetReturnValue().Set(v8::Number::New(isolate, v));
}

void SpinoWrapper::getDoubleValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    double v = obj->spino->getDoubleValue(*key);
    args.GetReturnValue().Set(v8::Number::New(isolate, v));
}

void SpinoWrapper::getStringValue(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    const char* v = obj->spino->getStringValue(*key);
    args.GetReturnValue().Set(String::NewFromUtf8(isolate, v).ToLocalChecked());
    delete v;
}

void SpinoWrapper::hasKey(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    v8::String::Utf8Value key(isolate, args[0]);

    SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());
    bool v = obj->spino->hasKey(*key);
    args.GetReturnValue().Set(v8::Boolean::New(isolate, v));
}

