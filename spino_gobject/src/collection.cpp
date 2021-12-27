#include "collection_private.h"
#include "cursor_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoCollection, spino_collection, G_TYPE_OBJECT)


enum
{
    PROP_0,
    LAST_PROP
};

enum
{
    LAST_SIGNAL
};

SpinoCollection* spino_collection_new(Spino::Collection* c)
{
    SpinoCollection* col = (SpinoCollection*)g_object_new(SPINO_TYPE_COLLECTION, NULL);
    col->priv = c;
    return col;
}

static void spino_collection_class_init(SpinoCollectionClass* klass) 
{

}

static void spino_collection_init(SpinoCollection* self) 
{

}

gchar* spino_collection_get_name(SpinoCollection* self)
{
    return g_strdup(self->priv->getName().c_str());
}


void spino_collection_create_index(SpinoCollection* self, const gchar* name) 
{
    self->priv->createIndex(name);
}


void spino_collection_drop_index(SpinoCollection* self, const gchar* name)
{
    self->priv->dropIndex(name);
}

void spino_collection_append(SpinoCollection* self, const gchar* doc)
{
    self->priv->append(doc);
}

void spino_collection_update_by_id(
        SpinoCollection* self, const gchar* id, const gchar* doc)
{
    self->priv->updateById(id, doc);
}

void spino_collection_update(
        SpinoCollection* self, const gchar* query, const gchar* doc)
{
    self->priv->update(query, doc);
}


gchar* spino_collection_find_one_by_id(SpinoCollection* self, const gchar* id)
{
    return g_strdup(self->priv->findOneById(id).c_str());
}

gchar* spino_collection_find_one(SpinoCollection* self, const gchar* query)
{
    return g_strdup(self->priv->findOne(query).c_str());
}

SpinoCursor* spino_collection_find(
        SpinoCollection* self, const gchar* query)
{
    auto* cursor = self->priv->find(query);
    return spino_cursor_new(cursor);
}


void spino_collection_drop_by_id(SpinoCollection* self, const gchar* id)
{
    self->priv->dropById(id);
}

void spino_collection_drop_one(SpinoCollection* self, const gchar* query)
{
    self->priv->dropOne(query);
}

guint spino_collection_drop(
        SpinoCollection* self, const gchar* query, uint32_t limit)
{
    return self->priv->drop(query, limit);
}

void spino_collection_drop_older_than(SpinoCollection* self, uint64_t timestamp)
{
    self->priv->dropOlderThan(timestamp);
}

uint64_t spino_collection_timestamp_by_id(SpinoCollection* self, const gchar* id)
{
    return self->priv->timestampById(id);
}

uint32_t spino_collection_get_size(SpinoCollection* self)
{
    return self->priv->size();
}


G_END_DECLS
