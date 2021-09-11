/* cppsrc/main.cpp */
#include "SpinoWrapper.h"
#include <napi.h>

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
	SpinoWrapper::Init(env, exports);
	CollectionWrapper::Init(env);
	CursorWrapper::Init(env);
	return exports;
}

NODE_API_MODULE(NODE_GYP_MODULE_NAME, InitAll)

