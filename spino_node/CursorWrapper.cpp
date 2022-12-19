#include "CursorWrapper.h"


Napi::Value dom_node_to_napi_value(Napi::Env env, const Spino::DomView* node);

Napi::FunctionReference* CursorWrapper::constructor;

Napi::Object CursorWrapper::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env,
                    "Cursor",
                    {
                        InstanceMethod("next", &CursorWrapper::next),
                        InstanceMethod("hasNext", &CursorWrapper::hasNext),
                        InstanceMethod("count", &CursorWrapper::count),
                        InstanceMethod("setLimit", &CursorWrapper::setLimit),
                        InstanceMethod("toArray", &CursorWrapper::toArray)
                    });

    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);
    env.SetInstanceData(constructor);

    return exports;
}

CursorWrapper::CursorWrapper(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<CursorWrapper>(info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsExternal())
    {
        Napi::TypeError::New(env, "External expected").ThrowAsJavaScriptException();
    }

    this->cursor = *info[0].As<Napi::External<shared_ptr<Spino::Cursor>>>().Data();
}

CursorWrapper::~CursorWrapper()
{
}

Napi::Value CursorWrapper::next(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (this->cursor->has_next())
    {
        return dom_node_to_napi_value(env, this->cursor->next_dom());
    }
    else
    {
        return env.Null();
    }
}

Napi::Value CursorWrapper::hasNext(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return Napi::Boolean::New(env, this->cursor->has_next());
}

Napi::Value CursorWrapper::count(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    return Napi::Number::New(env, this->cursor->count());
}

Napi::Value CursorWrapper::setLimit(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsNumber())
    {
        Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
    }

    Napi::Number limit = info[0].As<Napi::Number>();

    this->cursor->set_limit(limit.Int32Value());

    return env.Null();
}

Napi::Value CursorWrapper::toArray(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    Napi::Array array = Napi::Array::New(env);

    int i = 0;
    while (this->cursor->has_next())
    {
        auto obj = dom_node_to_napi_value(env, this->cursor->next_dom());
        array[i++] = obj;
    }

    return array;
}

Napi::Value CursorWrapper::runScript(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::HandleScope scope(env);

    if (info.Length() != 1 || !info[0].IsString())
    {
        Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
    }

    Napi::String data = info[0].As<Napi::String>();

    return Napi::String::New(env, this->cursor->run_script(data.Utf8Value()));
}
