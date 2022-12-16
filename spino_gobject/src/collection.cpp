#include "collection_private.h"
#include "cursor_private.h"
#include "document_node_private.h"
#include "document_viewer_private.h"

G_BEGIN_DECLS

G_DEFINE_TYPE(SpinoCollection, spino_collection, G_TYPE_OBJECT)

SpinoCollection *spino_collection_new(Spino::Collection *c)
{
    SpinoCollection *col = (SpinoCollection *)g_object_new(SPINO_TYPE_COLLECTION, NULL);
    col->priv = c;
    return col;
}

static void spino_collection_class_init(SpinoCollectionClass *klass)
{
}

static void spino_collection_init(SpinoCollection *self)
{
}

gchar *spino_collection_get_name(SpinoCollection *self)
{
    return g_strdup(self->priv->get_name().c_str());
}

void spino_collection_create_index(SpinoCollection *self, const gchar *name)
{
    self->priv->create_index(name);
}

void spino_collection_drop_index(SpinoCollection *self, const gchar *name)
{
    self->priv->drop_index(name);
}

void spino_collection_append(SpinoCollection *self, const gchar *doc)
{
    self->priv->append(doc);
}

void spino_collection_append_node(SpinoCollection *self, SpinoDocNode *node)
{
    self->priv->append(node->priv);
    node->priv = Spino::dom_node_allocator.make();
}

void spino_collection_upsert(
    SpinoCollection *self, const gchar *query, const gchar *doc)
{
    try {
        self->priv->upsert(query, doc);
    }
    catch(Spino::parse_error &err) {
        g_warning("Error parsing query: %s", err.what());
    }
    catch(...) {
        g_warning("Unknown error parsing query");
    }
}

void spino_collection_upsert_node(SpinoCollection *self, const gchar *query, SpinoDocNode *doc)
{
    try
    {
        self->priv->upsert(query, doc->priv);
        doc->priv = Spino::dom_node_allocator.make();
    }
    catch (Spino::parse_error &err)
    {
        g_warning("Error parsing query: %s", err.what());
    }
    catch (...)
    {
        g_warning("Unknown error parsing query");
    }
}

void spino_collection_update(SpinoCollection *self, const gchar *query, const gchar *doc)
{
    try
    {
        self->priv->update(query, doc);
    }
    catch (Spino::parse_error &err)
    {
        g_warning("Error parsing query: %s", err.what());
    }
    catch (...)
    {
        g_warning("Unknown error parsing query");
    }
}

void spino_collection_update_node(SpinoCollection *self, const gchar *query, SpinoDocNode *doc)
{
    try
    {
        self->priv->update(query, doc->priv);
        doc->priv = Spino::dom_node_allocator.make();
    }
    catch (Spino::parse_error &err)
    {
        g_warning("Error parsing query: %s", err.what());
    }
    catch (...)
    {
        g_warning("Unknown error parsing query");
    }
}

const gchar *spino_collection_find_one(SpinoCollection *self, const gchar *query)
{
    try
    {
        return g_strdup(self->priv->find_one(query).c_str());
    }
    catch (Spino::parse_error &err)
    {
        g_warning("Error parsing query: %s", err.what());
        return NULL;
    }
    catch (...)
    {
        g_warning("Unknown error parsing query");
        return NULL;
    }
}

SpinoDocView *spino_collection_find_one_view(SpinoCollection *self, const gchar *query)
{
    try
    {
        Spino::DomView *view = self->priv->find_one_dom(query);
        if (view != nullptr)
        {
            SpinoDocView *val = (SpinoDocView *)g_object_new(SPINO_TYPE_DOCVIEW, NULL);
            val->priv = view;
            return val;
        }
        return NULL;
    }
    catch (Spino::parse_error &err)
    {
        g_warning("Error parsing query: %s", err.what());
        return NULL;
    }
    catch (...)
    {
        g_warning("Unknown error parsing query");
        return NULL;
    }
}

SpinoCursor *spino_collection_find(
    SpinoCollection *self, const gchar *query)
{
    try
    {
        auto cursor = self->priv->find(query);
        return spino_cursor_new(cursor);
    }
    catch (Spino::parse_error &err)
    {
        g_warning("Error parsing query: %s", err.what());
        return NULL;
    }
    catch (...)
    {
        g_warning("Unknown error parsing query");
        return NULL;
    }
}

void spino_collection_drop_one(SpinoCollection *self, const gchar *query)
{
    try
    {
        self->priv->drop(query, 1);
    }
    catch (Spino::parse_error &err)
    {
        g_warning("Error parsing query: %s", err.what());
    }
    catch (...)
    {
        g_warning("Unknown error parsing query");
    }
}

guint spino_collection_drop(
    SpinoCollection *self, const gchar *query)
{
    try
    {
        return self->priv->drop(query);
    }
    catch (Spino::parse_error &err)
    {
        g_warning("Error parsing query: %s", err.what());
        return 0;
    }
    catch (...)
    {
        g_warning("Unknown error parsing query");
        return 0;
    }
}

uint32_t spino_collection_get_size(SpinoCollection *self)
{
    return self->priv->size();
}

G_END_DECLS
