#include "database_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoDatabase, spino_database, G_TYPE_OBJECT)


enum
{
    PROP_0,
    LAST_PROP
};

enum
{
    LAST_SIGNAL
};


SpinoDatabase* spino_database_new()
{
	return (SpinoDatabase*)g_object_new(SPINO_TYPE_DATABASE, NULL);
}


static void spino_database_finalize(GObject* object) 
{
    SpinoDatabase* self = (SpinoDatabase*)(object);
    delete self->db;
    G_OBJECT_CLASS(spino_database_parent_class)->finalize(object);
}


static void spino_database_class_init(SpinoDatabaseClass* klass) 
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = spino_database_finalize;    
}

static void spino_database_init(SpinoDatabase* self) 
{
    self->db = new Spino::SpinoDB(); 
}


gchar* spino_database_execute(SpinoDatabase* self, const gchar* command)
{
    return g_strdup(self->db->execute(command).c_str());
}


SpinoCollection* spino_database_add_collection(SpinoDatabase* self, const gchar* name)
{
    auto* col = self->db->addCollection(name);
    return spino_collection_new(col);
}


SpinoCollection* spino_database_get_collection(SpinoDatabase* self, const gchar* name)
{
    auto* col = self->db->getCollection(name);
    return spino_collection_new(col);
}


gboolean spino_database_has_collection(SpinoDatabase* self, const gchar* name)
{
    return self->db->hasCollection(name);
}

void spino_database_drop_collection(SpinoDatabase* self, const gchar* name)
{
    self->db->dropCollection(name);
}


void spino_database_save(SpinoDatabase* self)
{
    self->db->save();
}

void spino_database_load(SpinoDatabase* self)
{
    self->db->load();
}

void spino_database_set_paths(SpinoDatabase* self, const gchar* dbpath, const gchar* journalpath) 
{
    self->db->setPaths(dbpath, journalpath);
}

void spino_database_consolidate(SpinoDatabase* self) 
{
    self->db->consolidate();
}

void spino_database_set_int_value(SpinoDatabase* self, const gchar* key, gint value)
{
    self->db->setIntValue(key, value);
}

void spino_database_set_uint_value(SpinoDatabase* self, const gchar* key, guint value)
{
    self->db->setUintValue(key, value);
}

void spino_database_set_double_value(SpinoDatabase* self, const gchar* key, double value)
{
    self->db->setDoubleValue(key, value);
}

void spino_database_set_string_value(SpinoDatabase* self, const gchar* key, const gchar* value)
{
    self->db->setStringValue(key, value);
}

gint spino_database_get_int_value(SpinoDatabase* self, const gchar* key)
{
    return self->db->getIntValue(key);
}

guint spino_database_get_uint_value(SpinoDatabase* self, const gchar* key)
{
    return self->db->getUintValue(key);
}

double spino_database_get_double_value(SpinoDatabase* self, const gchar* key)
{
    return self->db->getDoubleValue(key);
}

const gchar* spino_database_get_string_value(SpinoDatabase* self, const gchar* key)
{
    return self->db->getStringValue(key);
}

gboolean spino_database_has_key(SpinoDatabase* self, const gchar* key)
{
    return self->db->hasKey(key);
}

G_END_DECLS
