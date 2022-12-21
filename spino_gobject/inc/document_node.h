#ifndef GSPINO_DOC_NODE_INCLUDED
#define GSPINO_DOC_NODE_INCLUDED

#include <glib-object.h>
#include <stdint.h>
#include "document_viewer.h"

G_BEGIN_DECLS

#define SPINO_TYPE_DOCNODE (spino_docnode_get_type())

G_DECLARE_FINAL_TYPE(SpinoDocNode, spino_docnode, Spino, DocNode, GObject)

/**
 * spino_docnode_new: (constructor)
 * Returns: (transfer full):  
 */
SpinoDocNode* spino_docnode_new();

void spino_docnode_from_json(SpinoDocNode* self, const gchar* json);
void spino_docnode_from_view(SpinoDocNode* self, SpinoDocView* view);
void spino_docnode_from_json_file(SpinoDocNode* self, const gchar* path);

void spino_docnode_set_object(SpinoDocNode* self);
void spino_docnode_set_array(SpinoDocNode* self);
void spino_docnode_set_int(SpinoDocNode* self, gint64 value);
void spino_docnode_set_uint(SpinoDocNode* self, guint64 value);
void spino_docnode_set_double(SpinoDocNode* self, double value);
void spino_docnode_set_bool(SpinoDocNode* self, gboolean value);
void spino_docnode_set_string(SpinoDocNode* self, const gchar* value);
void spino_docnode_set_null(SpinoDocNode* self);

void spino_docnode_add_int_member(SpinoDocNode* self, const gchar* name, const gint64 value);
void spino_docnode_add_uint_member(SpinoDocNode* self, const gchar* name, const guint64 value);
void spino_docnode_add_double_member(SpinoDocNode* self, const gchar* name, const double value);
void spino_docnode_add_string_member(SpinoDocNode* self, const gchar* name, const gchar* value);
void spino_docnode_add_bool_member(SpinoDocNode* self, const gchar* name, const gboolean value);
void spino_docnode_add_member(SpinoDocNode* self, const gchar* name, SpinoDocNode* value);

void spino_docnode_remove_member(SpinoDocNode* self, const gchar* name);
void spino_docnode_append(SpinoDocNode* self, SpinoDocNode* other);
void spino_docnode_pop_back(SpinoDocNode* self);
void spino_docnode_remove(SpinoDocNode* self, SpinoValueIterator* iter);

/**
 * spino_docnode_get_view:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoDocView* spino_docnode_get_view(SpinoDocNode* self);

G_END_DECLS

#endif

