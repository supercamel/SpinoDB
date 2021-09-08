#ifndef SPINO_WRAPPER_H
#define SPINO_WRAPPER_H

#include <napi.h>
#include "SpinoDB.h"

class CursorWrapper: public Napi::ObjectWrap<CursorWrapper> {
	public:
		static Napi::Object Create(Napi::Env, Spino::BaseCursor* ptr);
		CursorWrapper(const Napi::CallbackInfo& info);
        ~CursorWrapper();

	private:
		Napi::Value next(const Napi::CallbackInfo& info);

        Spino::BaseCursor* cursor;
};



class CollectionWrapper: public Napi::ObjectWrap<CollectionWrapper> {
	public:
		static Napi::Object Create(Napi::Env env, Spino::Collection* ptr);
		CollectionWrapper(const Napi::CallbackInfo& info);

	private:
		static Napi::FunctionReference constructor;
		Napi::Value set_name(const Napi::CallbackInfo& info);
		Napi::Value get_name(const Napi::CallbackInfo& info);
		Napi::Value create_index(const Napi::CallbackInfo& info);
		Napi::Value append(const Napi::CallbackInfo& info);
        Napi::Value updateById(const Napi::CallbackInfo& info);
		Napi::Value update(const Napi::CallbackInfo& info);
		Napi::Value findOneById(const Napi::CallbackInfo& info);
		Napi::Value findOne(const Napi::CallbackInfo& info);
		Napi::Value find(const Napi::CallbackInfo& info);
		Napi::Value dropById(const Napi::CallbackInfo& info);
		Napi::Value dropOne(const Napi::CallbackInfo& info);
		Napi::Value drop(const Napi::CallbackInfo& info);

		Spino::Collection* collection;
};


class SpinoWrapper: public Napi::ObjectWrap<SpinoWrapper> {
	public:
		static Napi::Object Init(Napi::Env env, Napi::Object exports); 
		SpinoWrapper(const Napi::CallbackInfo& info); 
        ~SpinoWrapper() { delete sambodb; }

	private:
		static Napi::FunctionReference constructor; 
		Napi::Value save(const Napi::CallbackInfo& info); 
		Napi::Value load(const Napi::CallbackInfo& info); 
		Napi::Value add_collection(const Napi::CallbackInfo& info);
		Napi::Value get_collection(const Napi::CallbackInfo& info);
		Napi::Value drop_collection(const Napi::CallbackInfo& info);
		Spino::SpinoDB* sambodb; 
};

#endif


