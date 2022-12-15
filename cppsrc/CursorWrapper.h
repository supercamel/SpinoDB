#ifndef CURSOR_WRAPPER_H
#define CURSOR_WRAPPER_H

#include <napi.h>
#include "cursor.h"


class CursorWrapper : public Napi::ObjectWrap<CursorWrapper> 
{
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	CursorWrapper(const Napi::CallbackInfo &info);
	~CursorWrapper();
	// constructor function
	static Napi::FunctionReference* constructor;
private:
	Napi::Value next(const Napi::CallbackInfo &info);
	Napi::Value hasNext(const Napi::CallbackInfo &info);
	Napi::Value count(const Napi::CallbackInfo &info);
	Napi::Value setLimit(const Napi::CallbackInfo &info);
	Napi::Value toArray(const Napi::CallbackInfo &info);
    Napi::Value runScript(const Napi::CallbackInfo &info);

	shared_ptr<Spino::Cursor> cursor;
};


#endif
