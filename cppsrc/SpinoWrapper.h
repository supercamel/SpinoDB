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

#include <node.h>
#include <node_object_wrap.h>
#include "SpinoDB.h"


class CursorWrapper: public node::ObjectWrap {
	public:
		CursorWrapper(Spino::Cursor* cursor) : cursor(cursor) { }
		~CursorWrapper() { delete cursor; }

		static void Init(v8::Isolate* isolate);
		static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args, Spino::Cursor* cur);
	private:

		static void next(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void hasNext(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void toArray(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void count(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void setProjection(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void setLimit(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void runScript(const v8::FunctionCallbackInfo<v8::Value>& args);


		static v8::Global<v8::Function> constructor;
		Spino::Cursor* cursor;
};


class CollectionWrapper: public node::ObjectWrap {
	public:

		CollectionWrapper(Spino::Collection* collection) : collection(collection) { }

		static void Init(v8::Isolate* isolate);
		static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args, Spino::Collection* col);

	private:
		~CollectionWrapper() { }

		static void getName(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void createIndex(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void dropIndex(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void append(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void updateById(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void update(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void findOneById(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void findOne(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void find(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void dropById(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void dropOne(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void drop(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void dropOlderThan(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void timestampById(const v8::FunctionCallbackInfo<v8::Value>& args);




		static v8::Global<v8::Function> constructor;	
		Spino::Collection* collection;
};


class SpinoWrapper: public node::ObjectWrap {
	public:
		static void Init(v8::Local<v8::Object> exports);

	private:
		explicit SpinoWrapper();
		~SpinoWrapper() { delete spino; }

		static void New(const v8::FunctionCallbackInfo<v8::Value>& args);

		static void execute(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void save(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void load(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void enableJournal(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void disableJournal(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void consolidate(const v8::FunctionCallbackInfo<v8::Value>& args);

		static void addCollection(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void getCollection(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void dropCollection(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void setBoolValue(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void setIntValue(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void setUintValue(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void setDoubleValue(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void setStringValue(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void getBoolValue(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void getIntValue(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void getUintValue(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void getDoubleValue(const v8::FunctionCallbackInfo<v8::Value>& args);
        static void getStringValue(const v8::FunctionCallbackInfo<v8::Value>& args);

        static void hasKey(const v8::FunctionCallbackInfo<v8::Value>& args);

		Spino::SpinoDB* spino; 
};

#endif


