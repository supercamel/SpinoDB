#include "SpinoWrapper.h"

#include <iostream>
using namespace std;


using node::AddEnvironmentCleanupHook;
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


static void rapidjsonDocFromV8(Isolate* isolate, Local<Object>& obj, Spino::ValueType& doc, auto allocator) {
	Local<v8::Context> context = isolate->GetCurrentContext();
	auto property_names = obj->GetOwnPropertyNames(context).ToLocalChecked();

	auto rjsonObj = doc.GetObject();

	Spino::ValueType skey;
	Spino::ValueType sval;

	auto length = property_names->Length();
	for (uint32_t i = 0; i < length; ++i) {
		auto key = property_names->Get(context, i).ToLocalChecked();
		auto value = obj->Get(context, key).ToLocalChecked();

		if(value->IsString()) {
			/*
			   String::Utf8Value utf8_key(isolate, key);
			   String::Utf8Value utf8_value(isolate, value);

			   Spino::ValueType skey;
			   Spino::ValueType sval;

			   skey.SetString(*utf8_key, utf8_key.length(), allocator);
			   sval.SetString(*utf8_value, utf8_value.length(), allocator);

			   rjsonObj.AddMember(skey, sval, allocator);
			   */
		}
		else if(value->IsNumber()) {
			String::Utf8Value utf8_key(isolate, key);

			sval = Spino::ValueType(value->ToNumber(context).ToLocalChecked()->Value());
			skey.SetString(*utf8_key, utf8_key.length(), allocator);

			rjsonObj.AddMember(skey, sval, allocator);
		}
	}
}


void CursorWrapper::Init(Isolate* isolate){
	// Prepare constructor template
	Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate);
	tpl->SetClassName(String::NewFromUtf8(isolate, "Cursor").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	// Prototype
	NODE_SET_PROTOTYPE_METHOD(tpl, "next", next);

	Local<Context> context = isolate->GetCurrentContext();
	constructor.Reset(isolate, tpl->GetFunction(context).ToLocalChecked());

	AddEnvironmentCleanupHook(isolate, [](void*) {
			constructor.Reset();
			}, nullptr);

}

void CursorWrapper::NewInstance(const v8::FunctionCallbackInfo<v8::Value>& args, Spino::BaseCursor* cur) {
	Isolate* isolate = args.GetIsolate();

	auto cons = Local<Function>::New(isolate, constructor);
	auto context = isolate->GetCurrentContext();
	auto instance = cons->NewInstance(context).ToLocalChecked();
	auto curwrapper = new CursorWrapper(cur);
	curwrapper->Wrap(instance);
	args.GetReturnValue().Set(instance);
}

void CursorWrapper::next(const v8::FunctionCallbackInfo<v8::Value>& args) {
	Isolate* isolate = args.GetIsolate();
	CursorWrapper* curwrap = ObjectWrap::Unwrap<CursorWrapper>(args.Holder());
	auto next = curwrap->cursor->next();
	if(next != "") {
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, next.c_str()).ToLocalChecked());
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
	NODE_SET_PROTOTYPE_METHOD(tpl, "append", append);
	NODE_SET_PROTOTYPE_METHOD(tpl, "updateById", updateById);
	NODE_SET_PROTOTYPE_METHOD(tpl, "update", update);
	NODE_SET_PROTOTYPE_METHOD(tpl, "findOneById", findOneById);
	NODE_SET_PROTOTYPE_METHOD(tpl, "findOne", findOne);
	NODE_SET_PROTOTYPE_METHOD(tpl, "find", find);
	NODE_SET_PROTOTYPE_METHOD(tpl, "dropById", dropById);
	NODE_SET_PROTOTYPE_METHOD(tpl, "dropOne", dropOne);
	NODE_SET_PROTOTYPE_METHOD(tpl, "drop", drop);
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

void CollectionWrapper::append(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

	/*
	if(args[0]->IsString()) {
		v8::String::Utf8Value str(isolate, args[0]);

		auto idstr = obj->collection->append(*str);
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, idstr.c_str()).ToLocalChecked());
	} 
	else if(args[0]->IsObject()) {
	*/
		auto handle = args[0].As<v8::Object>();

		/*
		rapidjsonDocFromV8(isolate, handle, obj->collection->add_stub(), obj->collection->get_allocator());
		auto idstr = obj->collection->indexNewDoc();
		*/
		auto jsonobj = v8::JSON::Stringify(isolate->GetCurrentContext(), handle).ToLocalChecked();
		v8::String::Utf8Value s(isolate, jsonobj);
		obj->collection->append(*s);
		
//	}
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

	obj->collection->update(*findstr, *update);
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

	CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());
	auto f = obj->collection->findOne(*str);
	if(f != "") {
		args.GetReturnValue().Set(String::NewFromUtf8(isolate, f.c_str()).ToLocalChecked());
	}
}

void CollectionWrapper::find(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	v8::String::Utf8Value str(isolate, args[0]);

	CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());
	auto cursor = obj->collection->find(*str);
	CursorWrapper::NewInstance(args, cursor);
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

	CollectionWrapper* obj = ObjectWrap::Unwrap<CollectionWrapper>(args.Holder());

	obj->collection->drop(*findstr);
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

	NODE_SET_PROTOTYPE_METHOD(tpl, "save", save);
	NODE_SET_PROTOTYPE_METHOD(tpl, "load", load);
	NODE_SET_PROTOTYPE_METHOD(tpl, "addCollection", addCollection);
	NODE_SET_PROTOTYPE_METHOD(tpl, "getCollection", getCollection);
	NODE_SET_PROTOTYPE_METHOD(tpl, "dropCollection", dropCollection);

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

void SpinoWrapper::addCollection(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	v8::String::Utf8Value str(isolate, args[0]);

	SpinoWrapper* spinowrap = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

	auto col = spinowrap->spino->add_collection(*str);
	CollectionWrapper::NewInstance(args, col);
}

void SpinoWrapper::getCollection(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	v8::String::Utf8Value str(isolate, args[0]);

	SpinoWrapper* spinowrap = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

	auto col = spinowrap->spino->get_collection(*str);
	CollectionWrapper::NewInstance(args, col);
}


void SpinoWrapper::dropCollection(const FunctionCallbackInfo<Value>& args) {
	Isolate* isolate = args.GetIsolate();
	v8::String::Utf8Value str(isolate, args[0]);

	SpinoWrapper* obj = ObjectWrap::Unwrap<SpinoWrapper>(args.Holder());

	obj->spino->drop_collection(*str);
}

