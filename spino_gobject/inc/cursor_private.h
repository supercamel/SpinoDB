#ifndef SPINO_CURSOR_PRIVATE_H
#define SPINO_CURSOR_PRIVATE_H

#include "cursor.h"
#include "cppsrc/cursor.h"

G_BEGIN_DECLS

struct _SpinoCursor{
    GObject parent_instance;
    shared_ptr<Spino::Cursor> priv;
};

SpinoCursor* spino_cursor_new(shared_ptr<Spino::Cursor> cursor);

G_END_DECLS

#endif


