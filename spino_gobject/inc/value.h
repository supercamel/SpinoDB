#ifndef GSPINO_VALUE_INCLUDED
#define GSPINO_VALUE_INCLUDED

#include <glib-object.h>
#include <stdint.h>
#include "document_viewer.h"

G_BEGIN_DECLS

#define SPINO_TYPE_VALUE (spino_value_get_type())

G_DECLARE_FINAL_TYPE(SpinoValue, spino_value, Spino, Value, GObject)


void spino_value_set_object(SpinoValue* self);
void spino_value_set_array(SpinoValue* self);
void spino_value_set_int(SpinoValue* self, int value);
void spino_value_set_uint(SpinoValue* self, guint value);
void spino_value_set_double(SpinoValue* self, double value);
void spino_value_set_bool(SpinoValue* self, gboolean value);
void spino_value_set_string(SpinoValue* self, const gchar* value);
void spino_value_set_null(SpinoValue* self);

void spino_value_add_int_member(SpinoValue* self, const gchar* name, const int value);
void spino_value_add_uint_member(SpinoValue* self, const gchar* name, const guint value);
void spino_value_add_double_member(SpinoValue* self, const gchar* name, const double value);
void spino_value_add_string_member(SpinoValue* self, const gchar* name, const gchar* value);
void spino_value_add_bool_member(SpinoValue* self, const gchar* name, const gboolean value);
void spino_value_add_member(SpinoValue* self, const gchar* name, SpinoValue* value);

void spino_value_remove_member(SpinoValue* self, const gchar* name);
void spino_value_append(SpinoValue* self, SpinoValue* other);
void spino_value_pop_back(SpinoValue* self);
void spino_value_remove(SpinoValue* self, SpinoValueIterator* iter);

/**
 * spino_value_get_viewer:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoDocView* spino_value_get_viewer(SpinoValue* self);

G_END_DECLS

#endif

