#ifndef GSPINO_DOC_VIEW_PRIVATE_H
#define GSPINO_DOC_VIEW_PRIVATE_H

#include "cppsrc/dom_node.h"
#include "document_viewer.h"

G_BEGIN_DECLS

struct _SpinoDocView {
    GObject parent_instance;
    const Spino::DomView* priv;
};


struct _SpinoMemberIterator {
    GObject parent_instance;
    Spino::MemberIterator iter;
};

SpinoMemberIterator* spino_member_iterator_new();

struct _SpinoValueIterator {
    GObject parent_instance;
    Spino::ElementIterator iter;
};


SpinoValueIterator* spino_value_iterator_new();


G_END_DECLS


#endif

