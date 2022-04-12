#ifndef GSPINO_DOCOBJECT_PRIVATE_H_INCLUDED
#define GSPINO_DOCOBJECT_PRIVATE_H_INCLUDED

#include "doc_object.h"
#include "cppsrc/QueryExecutor.h"

G_BEGIN_DECLS

struct _SpinoDocObject{
    GObject parent_instance;
    Spino::DocType* doc;
};


G_END_DECLS


#endif

