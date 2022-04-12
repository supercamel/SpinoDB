#ifndef GSPINO_VALUE_PRIVATE_H
#define GSPINO_VALUE_PRIVATE_H

#include "cppsrc/QueryExecutor.h"
#include "value.h"

G_BEGIN_DECLS

struct _SpinoValue {
    GObject parent_instance;
    Spino::ValueType priv;
    rapidjson::CrtAllocator* alloc;
};

SpinoValue* spino_value_new(rapidjson::CrtAllocator* a);

G_END_DECLS


#endif

