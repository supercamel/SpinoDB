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

void spino_database_drop_collection(SpinoDatabase* self, const gchar* name)
{
    self->db->dropCollection(name);
}


void spino_database_save(SpinoDatabase* self, const gchar* path)
{
    self->db->save(path);
}

void spino_database_load(SpinoDatabase* self, const gchar* path)
{
    self->db->load(path);
}



G_END_DECLS
