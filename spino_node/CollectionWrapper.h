#ifndef COLLECTION_WRAPPER_H
#define COLLECTION_WRAPPER_H

#include <napi.h>
#include "collection.h"

class CollectionWrapper : public Napi::ObjectWrap<CollectionWrapper>
{
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	CollectionWrapper(const Napi::CallbackInfo &info);
	~CollectionWrapper();
	// constructor function
	static Napi::FunctionReference* constructor;
private:
	Napi::Value getName(const Napi::CallbackInfo &info);
	Napi::Value createIndex(const Napi::CallbackInfo &info);
	Napi::Value dropIndex(const Napi::CallbackInfo &info);
	Napi::Value findOne(const Napi::CallbackInfo& info);
    Napi::Value find(const Napi::CallbackInfo& info);
	Napi::Value append(const Napi::CallbackInfo &info);
    Napi::Value upsert(const Napi::CallbackInfo& info);
    Napi::Value update(const Napi::CallbackInfo& info);
    Napi::Value drop(const Napi::CallbackInfo& info);
    Napi::Value size(const Napi::CallbackInfo& info);

	Spino::Collection *collection;
};

#endif
