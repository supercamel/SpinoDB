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


#include "Journal.h"

#include <iostream>
#include <fstream>
using namespace std;


namespace Spino {


    JournalWriter::JournalWriter() {
        path = "";
        enabled = false;
    }

    void JournalWriter::append(const std::string& s) {
        if(enabled == true) {
            fstream f(path, fstream::out | fstream::app);

            if(f.is_open())
            {
                f << s << endl;
                f.close();
            }
            else
            {
                cout << "SpinoDB:: Error writing to journal" << endl;;
            }
        }
    }

    void JournalWriter::setEnabled(bool e) {
        enabled = e;
    }

    bool JournalWriter::getEnabled() const {
        return enabled;
    }

    const char* JournalWriter::getPath() const {
        return path.c_str();
    }

    void JournalWriter::setPath(const std::string& p) {
        path = p;
    }
}


