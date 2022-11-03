#ifndef GSPINO_CURSOR_H_INCLUDED
#define GSPINO_CURSOR_H_INCLUDED

#include <glib-object.h>

G_BEGIN_DECLS

#define SPINO_TYPE_CURSOR (spino_cursor_get_type())

G_DECLARE_FINAL_TYPE(SpinoCursor, spino_cursor, Spino, Cursor, GObject)

/**
 * spino_cursor_next:
 * @self: the self
 * Returns: (transfer full):
 */
gchar* spino_cursor_next(SpinoCursor* self);
gboolean spino_cursor_has_next(SpinoCursor* self);

void spino_cursor_set_limit(SpinoCursor* self, guint limit);

guint spino_cursor_count(SpinoCursor* self);

G_END_DECLS

#endif


