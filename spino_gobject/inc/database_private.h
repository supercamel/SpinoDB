#ifndef SPINO_DATABASE_PRIVATE_H_INCLUDED
#define SPINO_DATABASE_PRIVATE_H_INCLUDED

#include "database.h"
#include "../../cppsrc/database.h"

struct _SpinoDatabase {
    GObject parent_instance;
    Spino::Database* db;
};



#endif

