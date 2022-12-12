#ifndef GSPINO_DOC_VIEW_INCLUDED
#define GSPINO_DOC_VIEW_INCLUDED

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



#define SPINO_TYPE_MEMBER_ITERATOR (spino_member_iterator_get_type())
G_DECLARE_FINAL_TYPE(SpinoMemberIterator, spino_member_iterator, Spino, MemberIterator, GObject)


#define SPINO_TYPE_VALUE_ITERATOR (spino_value_iterator_get_type())
G_DECLARE_FINAL_TYPE(SpinoValueIterator, spino_value_iterator, Spino, ValueIterator, GObject)


#define SPINO_TYPE_DOCVIEW (spino_docview_get_type())
G_DECLARE_FINAL_TYPE(SpinoDocView, spino_docview, Spino, DocView, GObject)



void spino_member_iterator_next(SpinoMemberIterator* iter);

/**
 * spino_member_iterator_get_name: 
 * @iter: the self
 * Returns: (transfer full):
 */
const gchar* spino_member_iterator_get_name(SpinoMemberIterator* iter);

/**
 * spino_member_iterator_get_view:
 * @iter: the self
 * Returns: (transfer full):
 */
SpinoDocView* spino_member_iterator_get_view(SpinoMemberIterator* iter);
gboolean spino_member_iterator_compare(SpinoMemberIterator* iter, SpinoMemberIterator* other);
gboolean spino_member_iterator_is_not(SpinoMemberIterator* iter, SpinoMemberIterator* other);


void spino_value_iterator_next(SpinoValueIterator* iter);
/**
 * spino_value_iterator_get_view:
 * @iter: the self
 * Returns: (transfer full):
 */
SpinoDocView* spino_value_iterator_get_view(SpinoValueIterator* iter);
gboolean spino_value_iterator_compare(SpinoValueIterator* iter, SpinoValueIterator* other);
gboolean spino_value_iterator_is_not(SpinoValueIterator* iter, SpinoValueIterator* other);

gint64 spino_docview_get_int(SpinoDocView* self);
guint64 spino_docview_get_uint(SpinoDocView* self);
double spino_docview_get_double(SpinoDocView* self);
gboolean spino_docview_get_bool(SpinoDocView* self);
const gchar* spino_docview_get_string(SpinoDocView* self);

SpinoVALUETYPE spino_docview_get_value_type(SpinoDocView* self);

gboolean spino_docview_has_member(SpinoDocView* self, const gchar* name);
SpinoVALUETYPE spino_docview_get_member_type(SpinoDocView* self, const gchar* name);

int spino_docview_get_int_member(SpinoDocView* self, const gchar* name);
guint spino_docview_get_uint_member(SpinoDocView* self, const gchar* name);
gboolean spino_docview_get_bool_member(SpinoDocView* self, const gchar* name);
double spino_docview_get_double_member(SpinoDocView* self, const gchar* name);
const gchar* spino_docview_get_string_member(SpinoDocView* self, const gchar* name);

/**
 * spino_docview_get_member:
 * @self: the self
 * @name: the json object key to retrieve
 * Returns: (transfer full):
 */
SpinoDocView* spino_docview_get_member(SpinoDocView* self, const gchar* name);

/**
 * spino_docview_member_begin:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoMemberIterator* spino_docview_member_begin(SpinoDocView* self);

/**
 * spino_docview_member_end:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoMemberIterator* spino_docview_member_end(SpinoDocView* self);

/**
 * spino_docview_get_element:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoDocView* spino_docview_get_element(SpinoDocView* self, guint index);

/**
 * spino_docview_begin:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoValueIterator* spino_docview_begin(SpinoDocView* self);

/**
 * spino_docview_end:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoValueIterator* spino_docview_end(SpinoDocView* self);

gboolean spino_docview_compare(SpinoDocView* self, SpinoDocView* other);

gchar* spino_docview_stringify(SpinoDocView* self);

G_END_DECLS

#endif



