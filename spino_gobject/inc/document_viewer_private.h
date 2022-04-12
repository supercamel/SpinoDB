#ifndef GSPINO_DOC_VIEW_PRIVATE_H
#define GSPINO_DOC_VIEW_PRIVATE_H

#include "cppsrc/QueryExecutor.h"
#include "document_viewer.h"

G_BEGIN_DECLS

struct _SpinoDocView {
    GObject parent_instance;
    const Spino::ValueType* priv;
};


struct _SpinoMemberIterator {
    GObject parent_instance;
    Spino::ValueType::ConstMemberIterator iter;
};

SpinoMemberIterator* spino_member_iterator_new();

struct _SpinoValueIterator {
    GObject parent_instance;
    Spino::ValueType::ConstValueIterator iter;
};


SpinoValueIterator* spino_value_iterator_new();


G_END_DECLS


#endif

