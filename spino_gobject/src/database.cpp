#include "database_private.h"
#include "collection_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoDatabase, spino_database, G_TYPE_OBJECT)



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
    self->db = new Spino::Database(); 
}


gchar* spino_database_execute(SpinoDatabase* self, const gchar* command)
{
    std::string result = self->db->execute(command);
    return g_strdup(result.c_str());
}


SpinoCollection* spino_database_get_collection(SpinoDatabase* self, const gchar* name)
{
    auto col = self->db->get_collection(name);
    return spino_collection_new(col);
}


gboolean spino_database_has_collection(SpinoDatabase* self, const gchar* name)
{
    return self->db->has_collection(name);
}

void spino_database_drop_collection(SpinoDatabase* self, const gchar* name)
{
    self->db->drop_collection(name);
}


void spino_database_save(SpinoDatabase* self, const gchar* path)
{
    self->db->save(path);
}

void spino_database_load(SpinoDatabase* self, const gchar* path)
{
    self->db->load(path);
}

void spino_database_enable_journal(SpinoDatabase* self, const gchar* journal_path)
{
    self->db->enable_journal(journal_path);
}

void spino_database_disable_journal(SpinoDatabase* self)
{
    self->db->disable_journal();
}

void spino_database_consolidate(SpinoDatabase* self, const gchar* db_path) 
{
    self->db->consolidate(db_path);
}

void spino_database_set_bool_value(SpinoDatabase* self, const gchar* key, gboolean value)
{
    self->db->set_bool_value(key, value);
}

void spino_database_set_int_value(SpinoDatabase* self, const gchar* key, gint value)
{
    self->db->set_int_value(key, value);
}

void spino_database_set_uint_value(SpinoDatabase* self, const gchar* key, guint value)
{
    self->db->set_uint_value(key, value);
}

void spino_database_set_double_value(SpinoDatabase* self, const gchar* key, double value)
{
    self->db->set_double_value(key, value);
}

void spino_database_set_string_value(SpinoDatabase* self, const gchar* key, const gchar* value)
{
    self->db->set_string_value(key, value);
}

gboolean spino_database_get_bool_value(SpinoDatabase* self, const gchar* key)
{
    return self->db->get_bool_value(key);
}

gint spino_database_get_int_value(SpinoDatabase* self, const gchar* key)
{
    return self->db->get_int_value(key);
}

guint spino_database_get_uint_value(SpinoDatabase* self, const gchar* key)
{
    return self->db->get_uint_value(key);
}

double spino_database_get_double_value(SpinoDatabase* self, const gchar* key)
{
    return self->db->get_double_value(key);
}

const gchar* spino_database_get_string_value(SpinoDatabase* self, const gchar* key)
{
    const char* value = self->db->get_string_value(key);
    return g_strdup(value);
}

gboolean spino_database_has_key(SpinoDatabase* self, const gchar* key)
{
    return self->db->has_key(key);
}

G_END_DECLS
