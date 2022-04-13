#ifndef GSPINO_VALUE_PRIVATE_H
#define GSPINO_VALUE_PRIVATE_H

#include "cppsrc/QueryExecutor.h"
#include "document_node.h"

G_BEGIN_DECLS

struct _SpinoDocNode {
    GObject parent_instance;
    Spino::ValueType priv;
    rapidjson::CrtAllocator* alloc;
};

SpinoDocNode* spino_docnode_new(rapidjson::CrtAllocator* a);

G_END_DECLS


#endif

