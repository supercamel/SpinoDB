#ifndef GSPINO_VALUE_PRIVATE_H
#define GSPINO_VALUE_PRIVATE_H

#include "document_node.h"
#include "cppsrc/dom_node.h"

G_BEGIN_DECLS

struct _SpinoDocNode {
    GObject parent_instance;
    Spino::DomNode* priv;
};


G_END_DECLS


#endif

