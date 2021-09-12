#ifndef SPINO_WRAPPER_H
#define SPINO_WRAPPER_H

#include <node.h>
#include <node_object_wrap.h>
#include "SpinoDB.h"


class CursorWrapper: public node::ObjectWrap {
	public:
		CursorWrapper(Spino::BaseCursor* cursor) : cursor(cursor) { }
		~CursorWrapper() { delete cursor; }

		static void Init(v8::Isolate* isolate);
		static void NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args, Spino::BaseCursor* cur);
	private:

		static void next(const v8::FunctionCallbackInfo<v8::Value>& args);

		static v8::Global<v8::Function> constructor;
		Spino::BaseCursor* cursor;
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
		static void addCollection(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void getCollection(const v8::FunctionCallbackInfo<v8::Value>& args);
		static void dropCollection(const v8::FunctionCallbackInfo<v8::Value>& args);

		Spino::SpinoDB* spino; 
};

#endif


