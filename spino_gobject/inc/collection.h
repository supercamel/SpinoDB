#ifndef GSPINO_COLLECTION_H_INCLUDED
#define GSPINO_COLLECTION_H_INCLUDED

#include <glib-object.h>
#include <stdint.h>
#include "cursor.h"
#include "document_node.h"

G_BEGIN_DECLS

#define SPINO_TYPE_COLLECTION (spino_collection_get_type())

G_DECLARE_FINAL_TYPE(SpinoCollection, spino_collection, Spino, Collection, GObject)

/**
 * spino_collection_get_name:
 * @self: the self
 * Returns: (transfer full):
 */
gchar* spino_collection_get_name(SpinoCollection* self);
void spino_collection_create_index(SpinoCollection* self, const gchar* name);
void spino_collection_drop_index(SpinoCollection* self, const gchar* name);
void spino_collection_append(SpinoCollection* self, const gchar* doc);
void spino_collection_append_node(SpinoCollection* self, SpinoDocNode* doc);
void spino_collection_update_by_id(SpinoCollection* self, const gchar* id, const gchar* doc);
void spino_collection_update(SpinoCollection* self, const gchar* query, const gchar* doc);

/**
 * spino_collection_find_one_by_id:
 * @self: the self
 * @id: the id of the item to find
 * Returns: (transfer full):
 */
gchar* spino_collection_find_one_by_id(SpinoCollection* self, const gchar* id);

/**
 * spino_collection_find_one:
 * @self: the self
 * @query: the query string
 * Returns: (transfer full):
 */
gchar* spino_collection_find_one(SpinoCollection* self, const gchar* query);

/**
 * spino_collection_find_one_view:
 * @self: the self
 * @query: the query string
 * Returns: (transfer full):
 */
SpinoDocView* spino_collection_find_one_view(SpinoCollection* self, const gchar* query);

/**
 * spino_collection_find:
 * @self: the self
 * @query: the query string
 * Returns: (transfer full):
 */
SpinoCursor* spino_collection_find(SpinoCollection* self, const gchar* query);

void spino_collection_drop_by_id(SpinoCollection* self, const gchar* id);
void spino_collection_drop_one(SpinoCollection* self, const gchar* query);
guint spino_collection_drop(SpinoCollection* self, const gchar* query, uint32_t limit);
void spino_collection_drop_older_than(SpinoCollection* self, uint64_t timestamp);
uint64_t spino_collection_timestamp_by_id(SpinoCollection* self, const gchar* id);

uint32_t spino_collection_get_size(SpinoCollection* self);

/**
 * spino_collection_create_node:
 * @self: the self
 * Returns: (transfer full):
 */
SpinoDocNode* spino_collection_create_node(SpinoCollection* self);


G_END_DECLS

#endif


