#ifndef SPINO_SQUIRREL_H
#define SPINO_SQUIRREL_H

#include "QueryNodes.h"
#include "squirrel.h"

namespace Spino {

    void squirrelPushJsonObj(HSQUIRRELVM vm, const ValueType& jsonObj);
    void squirrelPushJsonArr(HSQUIRRELVM vm, const ValueType& jsonArr);

    void squirrelPrintFunc(HSQUIRRELVM vm, const SQChar* s, ...);
    void squirrelErrorFunc(HSQUIRRELVM vm, const SQChar* s, ...);
    void squirrelCompileError(HSQUIRRELVM vm, const SQChar* desc,
            const SQChar* source, SQInteger line, SQInteger column);

    void loadSquirrelStdLib(HSQUIRRELVM vm);

}

#endif


