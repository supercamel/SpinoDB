#include "collection_private.h"
#include "cursor_private.h"
#include "document_node_private.h"
#include "document_viewer_private.h"

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

void spino_collection_append_node(SpinoCollection* self, SpinoDocNode* doc)
{
    self->priv->append(doc->priv);
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

SpinoDocView* spino_collection_find_one_view(SpinoCollection* self, const gchar* query)
{
    SpinoDocView* val = (SpinoDocView*)g_object_new(SPINO_TYPE_DOCVIEW, NULL);
     const Spino::ValueType* doc = self->priv->findOneValue(query);
    if(doc != nullptr) {
        val->priv = doc;
        return val;
    }
    else {
        return nullptr;
    }
}

SpinoCursor* spino_collection_find(
        SpinoCollection* self, const gchar* query)
{
    auto* cursor = self->priv->find(query);
    return spino_cursor_new(cursor, self->priv->getAllocator());
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

SpinoDocNode* spino_collection_create_node(SpinoCollection* self)
{
    return spino_docnode_new(self->priv->getAllocator());
}


G_END_DECLS
