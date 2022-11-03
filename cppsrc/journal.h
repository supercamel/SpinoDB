#ifndef SPINO_JOURNAL_H
#define SPINO_JOURNAL_H

#include <string>
#include <iostream>
#include <fstream>

namespace Spino
{
    class JournalWriter
    {
    public:
        JournalWriter()
        {
            path = "";
            enabled = false;
        }

        void append(const std::string &cmd)
        {
            if (enabled == true)
            {
                std::fstream f(path, std::fstream::out | std::fstream::app);

                if (f.is_open())
                {
                    f << cmd << std::endl;
                    f.close();
                }
                else
                {
                    std::cout << "SpinoDB:: Error writing to journal" << std::endl;
                }
            }
        }

        void set_enabled(bool e)
        {
            enabled = e;
        }

        bool get_enabled() const
        {
            return enabled; 
        }

        void set_path(const std::string &_path)
        {
            path = _path;
        }

        const char *get_path() const
        {
            return path.c_str();
        }

    private:
        std::string path;
        bool enabled;
    };
};

#endif
