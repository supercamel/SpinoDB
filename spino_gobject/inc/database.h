#ifndef SPINO_DATABASE_H_INCLUDED
#define SPINO_DATABASE_H_INCLUDED

#include <glib-object.h>

#include "collection.h"

G_BEGIN_DECLS

#define SPINO_TYPE_DATABASE (spino_database_get_type())

G_DECLARE_FINAL_TYPE(SpinoDatabase, spino_database, Spino, Database, GObject)

SpinoDatabase* spino_database_new();

gchar* spino_database_execute(SpinoDatabase* self, const gchar* command);


/**
 * spino_database_add_collection:
 * @self: the self
 * @name: the name of the collection
 * Returns: (transfer full):
 */
SpinoCollection* spino_database_add_collection(SpinoDatabase* self, const gchar* name);

/**
 * spino_database_get_collection:
 * @self: the self
 * @name: the name of the collection
 * Returns: (transfer full):
 */
SpinoCollection* spino_database_get_collection(SpinoDatabase* self, const gchar* name);

/**
 * spino_database_drop_collection:
 * @self: the self
 * @name: the name of the collection to drop
 */
void spino_database_drop_collection(SpinoDatabase* self, const gchar* name);

/**
 * spino_database_save:
 * @self: the self
 * @path: the file name to save to
 */
void spino_database_save(SpinoDatabase* self, const gchar* path);

/**
 * spino_database_load:
 * @self: the self
 * @path: the file name to load from 
 */
void spino_database_load(SpinoDatabase* self, const gchar* path);


G_END_DECLS

#endif


