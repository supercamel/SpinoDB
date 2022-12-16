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
#include "CollectionWrapper.h"

Napi::Object DatabaseWrapper::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env,
                    "Spino",
                    {InstanceMethod("getCollection", &DatabaseWrapper::getCollection),
                    InstanceMethod("hasCollection", &DatabaseWrapper::hasCollection),
                    InstanceMethod("dropCollection", &DatabaseWrapper::dropCollection),
                    InstanceMethod("setBoolValue", &DatabaseWrapper::setBoolValue),
                    InstanceMethod("getBoolValue", &DatabaseWrapper::getBoolValue),
                    InstanceMethod("setIntValue", &DatabaseWrapper::setIntValue),
                    InstanceMethod("getIntValue", &DatabaseWrapper::getIntValue),
                    InstanceMethod("setUintValue", &DatabaseWrapper::setUintValue),
                    InstanceMethod("getUintValue", &DatabaseWrapper::getUintValue),
                    InstanceMethod("setDoubleValue", &DatabaseWrapper::setDoubleValue),
                    InstanceMethod("getDoubleValue", &DatabaseWrapper::getDoubleValue),
                    InstanceMethod("setStringValue", &DatabaseWrapper::setStringValue),
                    InstanceMethod("getStringValue", &DatabaseWrapper::getStringValue),
                    InstanceMethod("hasKey", &DatabaseWrapper::hasKey),
                    InstanceMethod("save", &DatabaseWrapper::save),
                    InstanceMethod("load", &DatabaseWrapper::load),
                    InstanceMethod("enableJournal", &DatabaseWrapper::enableJournal),
                    InstanceMethod("disableJournal", &DatabaseWrapper::disableJournal),
                    InstanceMethod("consolidate", &DatabaseWrapper::consolidate),
                    InstanceMethod("execute", &DatabaseWrapper::execute)
                    });

    Napi::FunctionReference *constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    exports.Set("Spino", func);
    return exports;
}

DatabaseWrapper::DatabaseWrapper(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<DatabaseWrapper>(info)
{
    this->db = new Spino::Database();
}

DatabaseWrapper::~DatabaseWrapper()
{
    delete this->db;
}

Napi::Value DatabaseWrapper::getCollection(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    Spino::Collection *collection = this->db->get_collection(name.Utf8Value());

    // Create an object that wraps the collection
    Napi::Object obj = CollectionWrapper::constructor->New({Napi::External<Spino::Collection>::New(env, collection)});

    // Return the object
    return obj;
}

Napi::Value DatabaseWrapper::hasCollection(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    bool hasCollection = this->db->has_collection(name.Utf8Value());

    // Return the object
    return Napi::Boolean::New(env, hasCollection);
}

Napi::Value DatabaseWrapper::dropCollection(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    this->db->drop_collection(name.Utf8Value());

    // Return the object
    return env.Undefined();
}

Napi::Value DatabaseWrapper::setBoolValue(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2 || !info[0].IsString() || !info[1].IsBoolean())
    {
        Napi::TypeError::New(env, "String and Boolean expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    Napi::Boolean value = info[1].As<Napi::Boolean>();
    this->db->set_bool_value(name.Utf8Value(), value.Value());

    // Return the object
    return env.Undefined();
}

Napi::Value DatabaseWrapper::getBoolValue(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    bool value = this->db->get_bool_value(name.Utf8Value());

    // Return the object
    return Napi::Boolean::New(env, value);
}

Napi::Value DatabaseWrapper::setIntValue(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2 || !info[0].IsString() || !info[1].IsNumber())
    {
        Napi::TypeError::New(env, "String and Number expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    Napi::Number value = info[1].As<Napi::Number>();
    this->db->set_int_value(name.Utf8Value(), value.Int32Value());

    return env.Undefined();
}

Napi::Value DatabaseWrapper::getIntValue(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    int value = this->db->get_int_value(name.Utf8Value());

    return Napi::Number::New(env, value);
}

Napi::Value DatabaseWrapper::setUintValue(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2 || !info[0].IsString() || !info[1].IsNumber())
    {
        Napi::TypeError::New(env, "String and Number expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    Napi::Number value = info[1].As<Napi::Number>();
    this->db->set_uint_value(name.Utf8Value(), value.Uint32Value());

    return env.Undefined();
}

Napi::Value DatabaseWrapper::getUintValue(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    unsigned int value = this->db->get_uint_value(name.Utf8Value());

    return Napi::Number::New(env, value);
}

Napi::Value DatabaseWrapper::setStringValue(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2 || !info[0].IsString() || !info[1].IsString())
    {
        Napi::TypeError::New(env, "String and String expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    Napi::String value = info[1].As<Napi::String>();
    this->db->set_string_value(name.Utf8Value(), value.Utf8Value());

    return env.Undefined();
}

Napi::Value DatabaseWrapper::getStringValue(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    std::string value = this->db->get_string_value(name.Utf8Value());

    return Napi::String::New(env, value);
}

Napi::Value DatabaseWrapper::setDoubleValue(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2 || !info[0].IsString() || !info[1].IsNumber())
    {
        Napi::TypeError::New(env, "String and Number expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    Napi::Number value = info[1].As<Napi::Number>();
    this->db->set_double_value(name.Utf8Value(), value.DoubleValue());

    return env.Undefined();
}

Napi::Value DatabaseWrapper::getDoubleValue(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    double value = this->db->get_double_value(name.Utf8Value());

    return Napi::Number::New(env, value);
}

Napi::Value DatabaseWrapper::hasKey(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String name = info[0].As<Napi::String>();
    bool value = this->db->has_key(name.Utf8Value());

    return Napi::Boolean::New(env, value);
}

Napi::Value DatabaseWrapper::save(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String path = info[0].As<Napi::String>();
    this->db->save(path.Utf8Value());

    return env.Undefined();
}

Napi::Value DatabaseWrapper::load(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String path = info[0].As<Napi::String>();
    this->db->load(path.Utf8Value());

    return env.Undefined();
}

Napi::Value DatabaseWrapper::consolidate(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String path = info[0].As<Napi::String>();

    try {
        this->db->consolidate(path);
    }
    catch(std::exception& err) {
        throw Napi::Error::New(env, err.what());
    }

    return env.Undefined();
}

Napi::Value DatabaseWrapper::enableJournal(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String path = info[0].As<Napi::String>();

    this->db->enable_journal(path);

    return env.Undefined();
}

Napi::Value DatabaseWrapper::disableJournal(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    this->db->disable_journal();

    return env.Undefined();
}

Napi::Value DatabaseWrapper::execute(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String query = info[0].As<Napi::String>();
    std::string result;
    try {
        result = this->db->execute(query.Utf8Value());
    }
    catch(std::exception& err) {
        throw Napi::Error::New(env, err.what());
    }

    return Napi::String::New(env, result);
}

Napi::Value DatabaseWrapper::Escape(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String query = info[0].As<Napi::String>();
    std::string result = Spino::escape(query.Utf8Value());

    return Napi::String::New(env, result);
}

Napi::Value DatabaseWrapper::Unescape(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String query = info[0].As<Napi::String>();
    std::string result = Spino::unescape(query.Utf8Value());

    return Napi::String::New(env, result);
}
