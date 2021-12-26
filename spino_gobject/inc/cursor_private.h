#ifndef SPINO_CURSOR_PRIVATE_H
#define SPINO_CURSOR_PRIVATE_H

#include "cursor.h"
#include "cppsrc/SpinoDB.h"

G_BEGIN_DECLS

struct _SpinoCursor{
    GObject parent_instance;
    Spino::BaseCursor* priv;
};

SpinoCursor* spino_cursor_new(Spino::BaseCursor* cursor);

G_END_DECLS

#endif


