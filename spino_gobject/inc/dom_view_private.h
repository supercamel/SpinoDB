#ifndef SPINO_DOM_VIEW_PRIVATE_H
#define SPINO_DOM_VIEW_PRIVATE_H

#include "dom_view.h"
#include "cppsrc/dom_node.h"

G_BEGIN_DECLS

struct _SpinoDomView {
    GObject parent_instance;
    Spino::DomView* priv;
};


SpinoDomView* spino_dom_view_new_with_view(Spino::DomView* view);

G_END_DECLS

#endif


