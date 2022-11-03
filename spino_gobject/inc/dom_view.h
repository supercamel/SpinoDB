#ifndef GSPINO_DOM_VIEW_H_INCLUDED
#define GSPINO_DOM_VIEW_H_INCLUDED

#include <glib-object.h>

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

#define SPINO_TYPE_DOM_VIEW (spino_dom_view_get_type())

G_DECLARE_FINAL_TYPE(SpinoDomView, spino_dom_view, SPINO, DOM_VIEW, GObject)

/**
 * spino_dom_view_new:
 * Returns: (transfer full): the data
 */
SpinoDomView* spino_dom_view_new();

gint spino_dom_view_get_int(SpinoDomView* self);
guint spino_dom_view_get_uint(SpinoDomView* self);
gdouble spino_dom_view_get_double(SpinoDomView* self);

/**
 * spino_dom_view_get_string:
 * @self: the self
 * Returns: (transfer full):
 */
gchar* spino_dom_view_get_string(SpinoDomView* self);
gboolean spino_dom_view_get_boolean(SpinoDomView* self);

SpinoVALUETYPE spino_dom_view_get_node_type(SpinoDomView* self);

gboolean spino_dom_view_has_member(SpinoDomView* self, const gchar* name);

/**
 * spino_dom_view_get_member:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoMemberIterator* spino_dom_view_member_begin(SpinoDomView* self);

/**
 * spino_dom_view_get_member:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoMemberIterator* spino_dom_view_member_end(SpinoDomView* self);

/**
 * spino_dom_view_get_member:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoValueIterator* spino_dom_view_value_begin(SpinoDomView* self);

/**
 * spino_dom_view_get_member:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoValueIterator* spino_dom_view_value_end(SpinoDomView* self);

/**
 * spino_dom_view_get_member:
 * @self: the self
 * Returns: (transfer full):
 */
gchar* spino_dom_view_stringify(SpinoDomView* self);



G_END_DECLS

#endif
