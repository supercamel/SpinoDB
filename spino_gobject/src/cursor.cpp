#include "cursor_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoCursor, spino_cursor, G_TYPE_OBJECT)


static void spino_cursor_finalize(GObject* object) 
{
    SpinoCursor* self = (SpinoCursor*)(object);
    self->priv.reset();
    G_OBJECT_CLASS(spino_cursor_parent_class)->finalize(object);
}


static void spino_cursor_class_init(SpinoCursorClass* klass) 
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = spino_cursor_finalize; 
}

static void spino_cursor_init(SpinoCursor* self) 
{
   
}

SpinoCursor* spino_cursor_new(shared_ptr<Spino::Cursor> cursor)
{
    SpinoCursor* cur = (SpinoCursor*)g_object_new(SPINO_TYPE_CURSOR, NULL);
    cur->priv = cursor;
    return cur;
}

gchar* spino_cursor_next(SpinoCursor* self)
{
    gchar* nxt = self->priv->next();
    return nxt;
}

gboolean spino_cursor_has_next(SpinoCursor* self) 
{
    return self->priv->has_next();
}

void spino_cursor_set_limit(SpinoCursor* self, guint limit)
{
    self->priv->set_limit(limit);
}

guint spino_cursor_count(SpinoCursor* self)
{
    return self->priv->count();
}

G_END_DECLS
