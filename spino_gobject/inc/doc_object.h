#ifndef GSPINO_DOCUMENT_BUILDER_H_INCLUDED
#define GSPINO_DOCUMENT_BUILDER_H_INCLUDED

#include <glib-object.h>
#include <stdint.h>

#include "doc_array.h"

G_BEGIN_DECLS

#define SPINO_TYPE_DOCOBJECT (spino_docobject_get_type())

G_DECLARE_FINAL_TYPE(SpinoDocObject, spino_docobject, Spino, DocObject, GObject)

SpinoDocObject* spino_docobject_new();

void spino_docobject_clear(SpinoDocObject* self);
void spino_docobject_add_int_member(SpinoDocObject* self, const gchar* name, const int value);
void spino_docobject_add_uint_member(SpinoDocObject* self, const gchar* name, const guint value);
void spino_docobject_add_double_member(SpinoDocObject* self, const gchar* name, const double value);
void spino_docobject_add_string_member(SpinoDocObject* self, const gchar* name, const gchar* value);
void spino_docobject_add_bool_member(SpinoDocObject* self, const gchar* name, const gboolean value);
void spino_docobject_add_object_member(SpinoDocObject* self, const gchar* name, const SpinoDocObject* value);
void spino_docobject_add_array_member(SpinoDocObject* self, const gchar* name, const SpinoDocArray* value);

gboolean spino_docobject_has_member(SpinoDocObject* self, const gchar* name);
SpinoVALUETYPE spino_docobject_get_member_type(SpinoDocObject* self, const gchar* name);

int spino_docobject_get_int_member(SpinoDocObject* self, const gchar* name);
guint spino_docobject_get_uint_member(SpinoDocObject* self, const gchar* name);
gboolean spino_docobject_get_bool_member(SpinoDocObject* self, const gchar* name);
double spino_docobject_get_double_member(SpinoDocObject* self, const gchar* name);
const gchar* spino_docobject_get_string_member(SpinoDocObject* self, const gchar* name);
//SpinoDocObject spino_docobject_get_object_member(SpinoDocObject* self, const gchar* name);

/**
 * spino_docobject_stringify:
 * @self: the self
 * Returns: (transfer full): a JSON string
 */
gchar* spino_docobject_stringify(SpinoDocObject* self);

G_END_DECLS

#endif

