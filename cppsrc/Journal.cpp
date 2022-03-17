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
        if(enabled) {
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


