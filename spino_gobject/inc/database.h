#ifndef SPINO_DATABASE_H_INCLUDED
#define SPINO_DATABASE_H_INCLUDED

#include <glib-object.h>
#include "collection.h"

G_BEGIN_DECLS

#define SPINO_TYPE_DATABASE (spino_database_get_type())

G_DECLARE_FINAL_TYPE(SpinoDatabase, spino_database, Spino, Database, GObject)

SpinoDatabase* spino_database_new();

/**
 * spino_database_execute:
 * @self: The database object
 * @command: The command to execute
 * Returns: (transfer full): The result of the command
 */
gchar* spino_database_execute(SpinoDatabase* self, const gchar* command);

/**
 * spino_database_get_collection:
 * @self: the self
 * @name: the name of the collection
 * Returns: (transfer full):
 */
SpinoCollection* spino_database_get_collection(SpinoDatabase* self, const gchar* name);


/**
 * spino_database_has_collection:
 * @self: the self
 * @name: the name of the collection
 * Returns: TRUE if the collection exists, FALSE if not
 */
gboolean spino_database_has_collection(SpinoDatabase* self, const gchar* name);

/**
 * spino_database_drop_collection:
 * @self: the self
 * @name: the name of the collection to drop
 */
void spino_database_drop_collection(SpinoDatabase* self, const gchar* name);

/**
 * spino_database_save:
 * @self: the self
 * @path: the path to the database file
 */
void spino_database_save(SpinoDatabase* self, const gchar* path);

/**
 * spino_database_load:
 * @self: the self
 */
void spino_database_load(SpinoDatabase* self, const gchar* path);

/**
 * spino_database_enable_journal:
 * @self: the self
 * @journal_path: the path to the journal file
 */
void spino_database_enable_journal(SpinoDatabase* self, const gchar* journal_path);

/**
 * spino_database_disable_journal:
 * @self: the self
 */
void spino_database_disable_journal(SpinoDatabase* self);

/**
 * spino_database_consolidate:
 * @self: the self
 * @db_path: the path to the database file
 */
void spino_database_consolidate(SpinoDatabase* self, const gchar* db_path);

/**
 * spino_database_set_bool_value:
 * @self: the self
 * @key: the key
 * @value: the value to set
 */
void spino_database_set_bool_value(SpinoDatabase* self, const gchar* key, gboolean value);

/**
 * spino_database_set_int_value:
 * @self: the self
 * @key: the key
 * @value: the value to set
 */
void spino_database_set_int_value(SpinoDatabase* self, const gchar* key, gint value);

/**
 * spino_database_set_uint_value:
 * @self: the self
 * @key: the key
 * @value: the value to set
 */
void spino_database_set_uint_value(SpinoDatabase* self, const gchar* key, guint value);


/**
 * spino_database_set_double_value:
 * @self: the self
 * @key: the key
 * @value: the value to set
 */
void spino_database_set_double_value(SpinoDatabase* self, const gchar* key, double value);


/**
 * spino_database_set_string_value:
 * @self: the self
 * @key: the key
 * @value: the value to set
 */
void spino_database_set_string_value(SpinoDatabase* self, const gchar* key, const gchar* value);

/**
 * spino_database_get_bool_value:
 * @self: the self
 * @key: the key
 */
gboolean spino_database_get_bool_value(SpinoDatabase* self, const gchar* key);

/**
 * spino_database_get_int_value:
 * @self: the self
 * @key: the key
 */
int spino_database_get_int_value(SpinoDatabase* self, const gchar* key);

/**
 * spino_database_get_uint_value:
 * @self: the self
 * @key: the key
 */
guint spino_database_get_uint_value(SpinoDatabase* self, const gchar* key);

/**
 * spino_database_get_double_value:
 * @self: the self
 * @key: the key
 */
double spino_database_get_double_value(SpinoDatabase* self, const gchar* key);

/**
 * spino_database_get_string_value:
 * @self: the self
 * @key: the key
 * Returns: (transfer full): the string value
 */
const gchar* spino_database_get_string_value(SpinoDatabase* self, const gchar* key);

/**
 * spino_database_has_key:
 * @self: the self
 * @key: the key
 * Returns: true if the key exists
 */
gboolean spino_database_has_key(SpinoDatabase* self, const gchar* key);


G_END_DECLS

#endif


