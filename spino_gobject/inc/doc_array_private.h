#ifndef GSPINO_DOCARRAY_PRIVATE_H_INCLUDED
#define GSPINO_DOCARRAY_PRIVATE_H_INCLUDED

#include "doc_array.h"
#include "cppsrc/QueryExecutor.h"

G_BEGIN_DECLS

struct _SpinoDocArray {
    GObject parent_instance;
    Spino::DocType* doc;
};


G_END_DECLS


#endif

