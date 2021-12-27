#ifndef GSPINO_CURSOR_H_INCLUDED
#define GSPINO_CURSOR_H_INCLUDED

#include <glib-object.h>

G_BEGIN_DECLS

#define SPINO_TYPE_CURSOR (spino_cursor_get_type())

G_DECLARE_FINAL_TYPE(SpinoCursor, spino_cursor, Spino, Cursor, GObject)

gchar* spino_cursor_next(SpinoCursor* self);
guint spino_cursor_count(SpinoCursor* self);
gboolean spino_cursor_has_next(SpinoCursor* self);

/**
 * spino_cursor_set_projection:
 * @self: the self
 * @projection: the projection string
 * Returns: (transfer none):
 */
SpinoCursor* spino_cursor_set_projection(SpinoCursor* self, const gchar* projection);

/**
 * spino_cursor_set_limit:
 * @self: the self
 * @limit: the maximum number of documents the cursor can return 
 * Returns: (transfer none):
 */
SpinoCursor* spino_cursor_set_limit(SpinoCursor* self, guint limit);


G_END_DECLS

#endif


