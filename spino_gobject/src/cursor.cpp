#include "cursor_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoCursor, spino_cursor, G_TYPE_OBJECT)


enum
{
    PROP_0,
    LAST_PROP
};

enum
{
    LAST_SIGNAL
};


static void spino_cursor_class_init(SpinoCursorClass* klass) 
{

}

static void spino_cursor_init(SpinoCursor* self) 
{

}

SpinoCursor* spino_cursor_new(Spino::BaseCursor* cursor)
{
    SpinoCursor* cur = (SpinoCursor*)g_object_new(SPINO_TYPE_CURSOR, NULL);
    cur->priv = cursor;
    return cur;
}

gchar* spino_cursor_next(SpinoCursor* self)
{
    return g_strdup(self->priv->next().c_str());
}

guint spino_cursor_count(SpinoCursor* self)
{
    return self->priv->count();
}

gboolean spino_cursor_has_next(SpinoCursor* self) 
{
    return self->priv->hasNext();
}

SpinoCursor* spino_cursor_set_projection(SpinoCursor* self, const gchar* projection) 
{
    self->priv->setProjection(projection);
    return self;
}

SpinoCursor* spino_cursor_set_limit(SpinoCursor* self, guint limit)
{
    self->priv->setLimit(limit);
    return self;
}



G_END_DECLS
