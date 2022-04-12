#ifndef GSPINO_DOCUMENT_ARRAY_H_INCLUDED
#define GSPINO_DOCUMENT_ARRAY_H_INCLUDED

#include <glib-object.h>
#include <stdint.h>

G_BEGIN_DECLS

typedef enum {
    SPINO_VALUETYPE_INT,
    SPINO_VALUETYPE_UINT,
    SPINO_VALUETYPE_DOUBLE,
    SPINO_VALUETYPE_BOOL,
    SPINO_VALUETYPE_STRING,
    SPINO_VALUETYPE_ARRAY,
    SPINO_VALUETYPE_OBJECT,
    SPINO_VALUETYPE_NULL,
    SPINO_VALUETYPE_UNDEFINED
} SpinoVALUETYPE;

#define SPINO_VALUETYPE (spino_valuetype_get_type())

#define SPINO_TYPE_DOCARRAY (spino_docarray_get_type())

G_DECLARE_FINAL_TYPE(SpinoDocArray, spino_docarray, Spino, DocArray, GObject)

SpinoDocArray* spino_docarray_new();

struct _SpinoDocObject;

void spino_docarray_clear(SpinoDocArray* self);
void spino_docarray_add_int_element(SpinoDocArray* self, const int value);
void spino_docarray_add_uint_element(SpinoDocArray* self, const guint value);
void spino_docarray_add_double_element(SpinoDocArray* self, const double value);
void spino_docarray_add_string_element(SpinoDocArray* self, const gchar* value);
void spino_docarray_add_bool_element(SpinoDocArray* self, const gboolean value);
void spino_docarray_add_object_element(SpinoDocArray* self, _SpinoDocObject* value);
void spino_docarray_add_array_element(SpinoDocArray* self, SpinoDocArray* value);

/**
 * spino_docarray_stringify:
 * @self: the self
 * Returns: (transfer full): a JSON string
 */
gchar* spino_docarray_stringify(SpinoDocArray* self);


G_END_DECLS

#endif

