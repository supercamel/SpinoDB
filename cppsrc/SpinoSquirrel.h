//  Copyright 2022 Sam Cowen <samuel.cowen@camelsoftware.com>
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


