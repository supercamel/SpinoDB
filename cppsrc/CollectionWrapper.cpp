#include "CollectionWrapper.h"
#include "CursorWrapper.h"

Napi::FunctionReference *CollectionWrapper::constructor;

Napi::Object CollectionWrapper::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env,
                    "Collection",
                    {InstanceMethod("getName", &CollectionWrapper::getName),
                     InstanceMethod("createIndex", &CollectionWrapper::createIndex),
                     InstanceMethod("dropIndex", &CollectionWrapper::dropIndex),
                     InstanceMethod("findOne", &CollectionWrapper::findOne),
                     InstanceMethod("find", &CollectionWrapper::find),
                     InstanceMethod("append", &CollectionWrapper::append),
                     InstanceMethod("upsert", &CollectionWrapper::upsert),
                     InstanceMethod("update", &CollectionWrapper::update),
                     InstanceMethod("drop", &CollectionWrapper::drop),
                     InstanceMethod("size", &CollectionWrapper::size)});

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    return exports;
}

CollectionWrapper::CollectionWrapper(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<CollectionWrapper>(info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsExternal())
    {
        Napi::TypeError::New(env, "External expected").ThrowAsJavaScriptException();
    }

    this->collection = info[0].As<Napi::External<Spino::Collection>>().Data();
}

CollectionWrapper::~CollectionWrapper()
{
}

Napi::Value CollectionWrapper::getName(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return Napi::String::New(env, this->collection->get_name());
}

Napi::Value CollectionWrapper::createIndex(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String data = info[0].As<Napi::String>();
    this->collection->create_index(data.Utf8Value());

    return env.Null();
}

Napi::Value CollectionWrapper::dropIndex(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String data = info[0].As<Napi::String>();
    this->collection->drop_index(data.Utf8Value());

    return env.Null();
}

Napi::Value CollectionWrapper::findOne(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String data = info[0].As<Napi::String>();
    std::string result = this->collection->find_one(data.Utf8Value().c_str());

    return Napi::String::New(env, result);
}

Napi::Value CollectionWrapper::find(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String query = info[0].As<Napi::String>();
    shared_ptr<Spino::Cursor> cursor = this->collection->find(query.Utf8Value());

    // Create an object that wraps the collection
    Napi::Object obj = CursorWrapper::constructor->New({Napi::External<shared_ptr<Spino::Cursor>>::New(env, &cursor)});

    // Return the object
    return obj;
}

Napi::Value CollectionWrapper::append(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || (!info[0].IsString() && !info[0].IsObject()))
    {
        Napi::TypeError::New(env, "String or Object expected").ThrowAsJavaScriptException();
    }

    // if the argument is an object, stringify it
    if (info[0].IsObject())
    {
        Napi::Object json_obj = info[0].As<Napi::Object>();
        Napi::Object json = env.Global().Get("JSON").As<Napi::Object>();
        Napi::Function stringify = json.Get("stringify").As<Napi::Function>();
        Napi::String str = stringify.Call(json, {json_obj}).As<Napi::String>();
        this->collection->append(str.Utf8Value());
    }
    else
    {
        Napi::String data = info[0].As<Napi::String>();
        this->collection->append(data.Utf8Value());
    }

    return env.Null();
}


Napi::Value CollectionWrapper::upsert(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2 || !info[0].IsString() || !info[1].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String query = info[0].As<Napi::String>();
    Napi::String data = info[1].As<Napi::String>();
    this->collection->upsert(query.Utf8Value(), data.Utf8Value());

    return env.Null();
}

Napi::Value CollectionWrapper::update(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 2 || !info[0].IsString() || !info[1].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String query = info[0].As<Napi::String>();
    Napi::String data = info[1].As<Napi::String>();
    this->collection->update(query.Utf8Value(), data.Utf8Value());

    return env.Null();
}

Napi::Value CollectionWrapper::drop(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String query = info[0].As<Napi::String>();
    this->collection->drop(query.Utf8Value());

    return env.Null();
}

Napi::Value CollectionWrapper::size(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    int result = this->collection->size();

    return Napi::Number::New(env, result);
}
