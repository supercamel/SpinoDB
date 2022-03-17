#ifndef SPINO_JOURNAL_H
#define SPINO_JOURNAL_H

#include <string>


namespace Spino {

    class JournalWriter {
        public:
            JournalWriter();
            void append(const std::string& cmd);

            void setEnabled(bool e);
            bool getEnabled() const;

            void setPath(const std::string& path);
            const char* getPath() const;
        private:
            std::string path;
            bool enabled;
    };

}


#endif


