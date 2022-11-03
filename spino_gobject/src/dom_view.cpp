#include "dom_view_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoDomView, spino_dom_view, G_TYPE_OBJECT)


static void spino_dom_view_finalize(GObject* object) 
{
    SpinoDomView* self = (SpinoDomView*)(object);
    G_OBJECT_CLASS(spino_dom_view_parent_class)->finalize(object);
}


static void spino_cursor_class_init(SpinoCursorClass* klass) 
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = spino_cursor_finalize; 
}

static void spino_cursor_init(SpinoCursor* self) 
{
   
}

SpinoCursor* spino_cursor_new(Spino::Cursor* cursor)
{
    SpinoCursor* cur = (SpinoCursor*)g_object_new(SPINO_TYPE_CURSOR, NULL);
    cur->priv = cursor;
    return cur;
}
