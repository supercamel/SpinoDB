#include "collection_private.h"
#include "cursor_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoCollection, spino_collection, G_TYPE_OBJECT)


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
    return g_strdup(self->priv->get_name().c_str());
}


void spino_collection_create_index(SpinoCollection* self, const gchar* name) 
{
    self->priv->create_index(name);
}

void spino_collection_drop_index(SpinoCollection* self, const gchar* name)
{
    self->priv->drop_index(name);
}

void spino_collection_append(SpinoCollection* self, const gchar* doc)
{
    self->priv->append(doc);
}

void spino_collection_upsert(
        SpinoCollection* self, const gchar* query, const gchar* doc)
{
    self->priv->upsert(query, doc);
}

const gchar* spino_collection_find_one(SpinoCollection* self, const gchar* query)
{
    return g_strdup(self->priv->find_one(query));
}

SpinoCursor* spino_collection_find(
        SpinoCollection* self, const gchar* query)
{
    auto cursor = self->priv->find(query);
    return spino_cursor_new(cursor);
}

void spino_collection_drop_one(SpinoCollection* self, const gchar* query)
{
    self->priv->drop(query, 1);
}

guint spino_collection_drop(
        SpinoCollection* self, const gchar* query)
{
    return self->priv->drop(query);
}

uint32_t spino_collection_get_size(SpinoCollection* self)
{
    return self->priv->size();
}

G_END_DECLS
