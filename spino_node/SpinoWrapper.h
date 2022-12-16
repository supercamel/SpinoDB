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

#ifndef SPINO_WRAPPER_H
#define SPINO_WRAPPER_H

#include <napi.h>
#include "database.h"

class DatabaseWrapper : public Napi::ObjectWrap<DatabaseWrapper>
{
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);
	DatabaseWrapper(const Napi::CallbackInfo &info);
	~DatabaseWrapper();

	static Napi::Value Escape(const Napi::CallbackInfo &info);
	static Napi::Value Unescape(const Napi::CallbackInfo &info);

private:
	Napi::Value getCollection(const Napi::CallbackInfo &info);
	Napi::Value hasCollection(const Napi::CallbackInfo &info);
	Napi::Value dropCollection(const Napi::CallbackInfo &info);
	Napi::Value setBoolValue(const Napi::CallbackInfo& info);
	Napi::Value getBoolValue(const Napi::CallbackInfo& info);
	Napi::Value setIntValue(const Napi::CallbackInfo& info);
	Napi::Value getIntValue(const Napi::CallbackInfo& info);
	Napi::Value setUintValue(const Napi::CallbackInfo& info);
	Napi::Value getUintValue(const Napi::CallbackInfo& info);
	Napi::Value setStringValue(const Napi::CallbackInfo& info);
	Napi::Value getStringValue(const Napi::CallbackInfo& info);
	Napi::Value setDoubleValue(const Napi::CallbackInfo& info);
	Napi::Value getDoubleValue(const Napi::CallbackInfo& info);
	Napi::Value hasKey(const Napi::CallbackInfo& info);
	Napi::Value save(const Napi::CallbackInfo& info);
	Napi::Value load(const Napi::CallbackInfo& info);
	Napi::Value enableJournal(const Napi::CallbackInfo& info);
	Napi::Value disableJournal(const Napi::CallbackInfo& info);
	Napi::Value consolidate(const Napi::CallbackInfo& info);
	Napi::Value execute(const Napi::CallbackInfo& info);


	Spino::Database *db;
};

#endif
