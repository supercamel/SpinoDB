#ifndef GSPINO_DOC_NODE_INCLUDED
#define GSPINO_DOC_NODE_INCLUDED

#include <glib-object.h>
#include <stdint.h>
#include "document_viewer.h"

G_BEGIN_DECLS

#define SPINO_TYPE_DOCNODE (spino_docnode_get_type())

G_DECLARE_FINAL_TYPE(SpinoDocNode, spino_docnode, Spino, DocNode, GObject)


void spino_docnode_set_object(SpinoDocNode* self);
void spino_docnode_set_array(SpinoDocNode* self);
void spino_docnode_set_int(SpinoDocNode* self, int value);
void spino_docnode_set_uint(SpinoDocNode* self, guint value);
void spino_docnode_set_double(SpinoDocNode* self, double value);
void spino_docnode_set_bool(SpinoDocNode* self, gboolean value);
void spino_docnode_set_string(SpinoDocNode* self, const gchar* value);
void spino_docnode_set_null(SpinoDocNode* self);

void spino_docnode_add_int_member(SpinoDocNode* self, const gchar* name, const int value);
void spino_docnode_add_uint_member(SpinoDocNode* self, const gchar* name, const guint value);
void spino_docnode_add_double_member(SpinoDocNode* self, const gchar* name, const double value);
void spino_docnode_add_string_member(SpinoDocNode* self, const gchar* name, const gchar* value);
void spino_docnode_add_bool_member(SpinoDocNode* self, const gchar* name, const gboolean value);
void spino_docnode_add_member(SpinoDocNode* self, const gchar* name, SpinoDocNode* value);

void spino_docnode_remove_member(SpinoDocNode* self, const gchar* name);
void spino_docnode_append(SpinoDocNode* self, SpinoDocNode* other);
void spino_docnode_pop_back(SpinoDocNode* self);
void spino_docnode_remove(SpinoDocNode* self, SpinoValueIterator* iter);

/**
 * spino_docnode_get_viewer:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoDocView* spino_docnode_get_viewer(SpinoDocNode* self);

G_END_DECLS

#endif

