#include "cursor_private.h"
#include "document_viewer_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoCursor, spino_cursor, G_TYPE_OBJECT)


static void spino_cursor_class_init(SpinoCursorClass* klass) 
{

}

static void spino_cursor_init(SpinoCursor* self) 
{

}

SpinoCursor* spino_cursor_new(Spino::Cursor* cursor, rapidjson::CrtAllocator* alloc)
{
    SpinoCursor* cur = (SpinoCursor*)g_object_new(SPINO_TYPE_CURSOR, NULL);
    cur->priv = cursor;
    cur->alloc = alloc;
    return cur;
}

gchar* spino_cursor_next(SpinoCursor* self)
{
    return g_strdup(self->priv->next().c_str());
}

SpinoDocView* spino_cursor_next_view(SpinoCursor* self)
{
    SpinoDocView* val = (SpinoDocView*)g_object_new(SPINO_TYPE_DOCVIEW, NULL);
    val->priv = self->priv->nextAsJsonObj();
    return val;
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


gchar* spino_cursor_run_script(SpinoCursor* self, const gchar* script)
{
    return g_strdup(self->priv->runScript(script).c_str());
}

G_END_DECLS
