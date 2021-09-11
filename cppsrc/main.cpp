#include <node.h>
#include "SpinoWrapper.h"

namespace SpinoDB {

	using v8::Local;
	using v8::Object;

	void InitAll(Local<Object> exports) {
		CursorWrapper::Init(exports->GetIsolate());
		CollectionWrapper::Init(exports->GetIsolate());
		SpinoWrapper::Init(exports);
	}

	NODE_MODULE(NODE_GYP_MODULE_NAME, InitAll)

}
