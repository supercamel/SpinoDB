#include "SpinoWrapper.h"

Napi::FunctionReference CollectionWrapper::constructor;
Napi::FunctionReference SpinoWrapper::constructor;
Napi::FunctionReference CursorWrapper::constructor;


void CursorWrapper::Init(Napi::Env env) {
	Napi::HandleScope scope(env);
	Napi::Function func = DefineClass(env, "Cursor", {
			InstanceMethod("next", &CursorWrapper::next)
			});
	constructor = Napi::Persistent(func);

}

Napi::Object CursorWrapper::Create(Napi::Env env, Spino::BaseCursor* ptr) {
	auto ret = constructor.Value().New({});
	Unwrap(ret)->cursor = ptr;
	return ret;

}

CursorWrapper::CursorWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<CursorWrapper>(info)  {
	cursor = static_cast<Spino::BaseCursor*>(info.Data());
}

CursorWrapper::~CursorWrapper() {

	delete cursor;
}

Napi::Value CursorWrapper::next(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	std::string n = cursor->next();
	if(n.length() > 0) {
		return Napi::String::New(env, n.c_str());	
	}
	else {
		return Napi::Value();
	}
}

void CollectionWrapper::Init(Napi::Env env) {
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "Collection", {
			InstanceMethod("getName", &CollectionWrapper::get_name),
			InstanceMethod("createIndex", &CollectionWrapper::create_index),
			InstanceMethod("append", &CollectionWrapper::append),
			InstanceMethod("updateById", &CollectionWrapper::updateById),
			InstanceMethod("update", &CollectionWrapper::update),
			InstanceMethod("findOneById", &CollectionWrapper::findOneById),
			InstanceMethod("findOne", &CollectionWrapper::findOne),
			InstanceMethod("find", &CollectionWrapper::find),
			InstanceMethod("dropById", &CollectionWrapper::dropById),
			InstanceMethod("dropOne", &CollectionWrapper::dropOne),
			InstanceMethod("drop", &CollectionWrapper::drop)
			});

	constructor = Napi::Persistent(func);
}


Napi::Object CollectionWrapper::Create(Napi::Env env, Spino::Collection* ptr) {
	auto ret = constructor.Value().New({});
	Unwrap(ret)->collection = ptr;
	return ret;
}


CollectionWrapper::CollectionWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<CollectionWrapper>(info)  {
	collection = static_cast<Spino::Collection*>(info.Data());
}

Napi::Value CollectionWrapper::get_name(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	return Napi::String::New(env, collection->get_name());
}


Napi::Value CollectionWrapper::create_index(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 1) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}
	if(info[0].IsString()) {
		collection->create_index(info[0].As<Napi::String>().Utf8Value().c_str());
	} 
	else {
		Napi::TypeError::New(env, "Argument must be a string").ThrowAsJavaScriptException();
	}

	return Napi::Value();
}

Napi::Value CollectionWrapper::append(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 1) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}
	if(info[0].IsString()) {
		auto id = collection->append(info[0].As<Napi::String>().Utf8Value().c_str());
		return Napi::String::New(env, id.c_str());
	} 
	else if(info[0].IsObject()) {
		Napi::Object json_object = info[0].As<Napi::Object>();
		Napi::Object json = env.Global().Get("JSON").As<Napi::Object>();
		Napi::Function stringify = json.Get("stringify").As<Napi::Function>();
		auto str = stringify.Call(json, { json_object }).As<Napi::String>();
		auto id = collection->append(str.Utf8Value().c_str());
		return Napi::String::New(env, id.c_str());
	} 
	else {
		Napi::TypeError::New(env, "Argument can be either a string or an object").ThrowAsJavaScriptException();
	}

	return Napi::Value();
}

Napi::Value CollectionWrapper::updateById(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 2 || !info[0].IsString() || !info[1].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}


	try {
		collection->updateById(
				info[0].As<Napi::String>().Utf8Value().c_str(),
				info[1].As<Napi::String>().Utf8Value().c_str()
				);
	} catch(Spino::parse_error& e) {
		Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
	}
	return Napi::Value();
}


Napi::Value CollectionWrapper::update(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 2 || !info[0].IsString() || !info[1].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}


	try {
		collection->update(
				info[0].As<Napi::String>().Utf8Value().c_str(),
				info[1].As<Napi::String>().Utf8Value().c_str()
				);
	} catch(Spino::parse_error& e) {
		Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
	}
	return Napi::Value();
}

Napi::Value CollectionWrapper::findOneById(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}

	std::string result;
	try {
		result = collection->findOneById(info[0].As<Napi::String>().Utf8Value().c_str());
	} catch(Spino::parse_error& e) {
		Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
	}
	if(result.length() > 0) {
		return Napi::String::New(env, result.c_str());	
	} 
	else {
		return Napi::Value();
	}
}



Napi::Value CollectionWrapper::findOne(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}

	std::string result;
	try {
		result = collection->findOne(info[0].As<Napi::String>().Utf8Value().c_str());
	} catch(Spino::parse_error& e) {
		Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
	}
	if(result.length() > 0) {
		return Napi::String::New(env, result.c_str());	
	} 
	else {
		return Napi::Value();
	}
}

Napi::Value CollectionWrapper::find(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}

	auto cur = collection->find(info[0].As<Napi::String>().Utf8Value().c_str());
	if(cur == nullptr) {
		return Napi::Value();
	} else {
		auto curwrapper = Napi::Persistent(CursorWrapper::Create(info.Env(), cur));
		return curwrapper.Value();
	}
}

Napi::Value CollectionWrapper::dropById(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}

	try {
		collection->dropById(info[0].As<Napi::String>().Utf8Value().c_str());
	} catch(Spino::parse_error& e) {
		Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
	}
	return Napi::Value();

}

Napi::Value CollectionWrapper::dropOne(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}

	try {
		collection->dropOne(info[0].As<Napi::String>().Utf8Value().c_str());
	} catch(Spino::parse_error& e) {
		Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
	}
	return Napi::Value();
}

Napi::Value CollectionWrapper::drop(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}

	try {
		collection->drop(info[0].As<Napi::String>().Utf8Value().c_str());
	} catch(Spino::parse_error& e) {
		Napi::Error::New(env, e.what()).ThrowAsJavaScriptException();
	}
	return Napi::Value();
}

Napi::Object SpinoWrapper::Init(Napi::Env env, Napi::Object exports) {
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "Spino", {
			InstanceMethod("save", &SpinoWrapper::save),
			InstanceMethod("load", &SpinoWrapper::load),
			InstanceMethod("addCollection", &SpinoWrapper::add_collection),
			InstanceMethod("getCollection", &SpinoWrapper::get_collection),
			InstanceMethod("dropCollection", &SpinoWrapper::drop_collection)
			});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Spino", func);
	return exports;
}


SpinoWrapper::SpinoWrapper(const Napi::CallbackInfo& info) : Napi::ObjectWrap<SpinoWrapper>(info)  {
	sambodb = new Spino::SpinoDB();
}

Napi::Value SpinoWrapper::load(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	Napi::HandleScope scope(env);

	if (  info.Length() != 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}

	std::string path = info[0].As<Napi::String>().Utf8Value();
	sambodb->load(path);
	return Napi::Value();
}

Napi::Value SpinoWrapper::save(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if (  info.Length() != 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}

	std::string path = info[0].As<Napi::String>().Utf8Value();
	sambodb->save(path);
	return Napi::Value();
}

Napi::Value SpinoWrapper::add_collection(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}

	std::string name = info[0].As<Napi::String>().Utf8Value();
	auto col = sambodb->add_collection(name);
	auto colwrapper = Napi::Persistent(CollectionWrapper::Create(info.Env(), col));
	return colwrapper.Value();
}

Napi::Value SpinoWrapper::get_collection(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}

	std::string name = info[0].As<Napi::String>().Utf8Value();
	auto col = sambodb->get_collection(name);
	if(col == nullptr) {
		return Napi::Value();
	} 
	else {
		return Napi::Persistent(CollectionWrapper::Create(info.Env(), col)).Value();
	}
}

Napi::Value SpinoWrapper::drop_collection(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	if(info.Length() != 1 || !info[0].IsString()) {
		Napi::TypeError::New(env, "String expected").ThrowAsJavaScriptException();
	}

	std::string name = info[0].As<Napi::String>().Utf8Value();
	sambodb->drop_collection(name);
	return Napi::Value();
}

