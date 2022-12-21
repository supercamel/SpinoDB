#include "document_node_private.h"
#include "document_viewer_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoDocNode, spino_docnode, G_TYPE_OBJECT)

static void spino_docnode_finalize(GObject* object) 
{
    SpinoDocNode* self = (SpinoDocNode*)(object);
    if(self->priv != nullptr)  {
        self->priv->destroy();
        Spino::dom_node_allocator.delete_object(self->priv);
        self->priv = nullptr;
    }

    G_OBJECT_CLASS(spino_docnode_parent_class)->finalize(object);
}

static void spino_docnode_class_init(SpinoDocNodeClass* klass) 
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = spino_docnode_finalize;
}

static void spino_docnode_init(SpinoDocNode* self) 
{

}

SpinoDocNode* spino_docnode_new()
{
    SpinoDocNode* val = (SpinoDocNode*)g_object_new(SPINO_TYPE_DOCNODE, NULL);
    val->priv = Spino::dom_node_allocator.make();
    return val;
}

void spino_docnode_from_json(SpinoDocNode* self, const gchar* json)
{
    try {
        Spino::DomNode* dom = Spino::DomNode::from_json(json);
        self->priv->destroy();
        Spino::dom_node_allocator.delete_object(self->priv);
        self->priv = dom;
    }
    catch(std::exception& e) {
        g_warning("Error parsing json: %s", e.what());
        self->priv = Spino::dom_node_allocator.make();
    }
}

void spino_docnode_from_view(SpinoDocNode* self, SpinoDocView* view)
{
    self->priv->destroy();
    self->priv->copy(view->priv);
}

void spino_docnode_from_json_file(SpinoDocNode* self, const gchar* filename)
{
    try {
        Spino::DomNode* dom = Spino::DomNode::from_json_file(filename);

        self->priv->destroy();
        Spino::dom_node_allocator.delete_object(self->priv);
        self->priv = dom;
    }
    catch(...) {
        g_warning("Error parsing json");
        self->priv = Spino::dom_node_allocator.make();
    }
}

void spino_docnode_set_object(SpinoDocNode* self)
{
    self->priv->set_object();
}

void spino_docnode_set_array(SpinoDocNode* self)
{
    self->priv->set_array();
}

void spino_docnode_set_int(SpinoDocNode* self, gint64 value)
{
    self->priv->set_int(value);
}

void spino_docnode_set_uint(SpinoDocNode* self, guint64 value)
{
    self->priv->set_uint(value);
}

void spino_docnode_set_double(SpinoDocNode* self, double value) 
{
    self->priv->set_double(value);
}

void spino_docnode_set_bool(SpinoDocNode* self, gboolean value)
{
    self->priv->set_bool(value);
}

void spino_docnode_set_string(SpinoDocNode* self, const gchar* value)
{
    self->priv->set_string(value, strlen(value), true);
}

void spino_docnode_set_null(SpinoDocNode* self)
{
    self->priv->set_null();
}

void spino_docnode_add_int_member(SpinoDocNode* self, const gchar* name, const gint64 value)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        Spino::DomNode* d = Spino::dom_node_allocator.make();
        d->set_int(value);
        self->priv->add_member(name, d);
    }
}

void spino_docnode_add_uint_member(SpinoDocNode* self, const gchar* name, const guint64 value)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        Spino::DomNode* d = Spino::dom_node_allocator.make();
        d->set_uint(value);
        self->priv->add_member(name, d);
    }
}

void spino_docnode_add_double_member(SpinoDocNode* self, const gchar* name, const double value)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        Spino::DomNode* d = Spino::dom_node_allocator.make();
        d->set_double(value);
        self->priv->add_member(name, d);
    }
}

void spino_docnode_add_string_member(SpinoDocNode* self, const gchar* name, const gchar* value)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        Spino::DomNode* d = Spino::dom_node_allocator.make();
        d->set_string(value, strlen(value), true);
        self->priv->add_member(name, d);
    }
}

void spino_docnode_add_bool_member(SpinoDocNode* self, const gchar* name, const gboolean value)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        Spino::DomNode* d = Spino::dom_node_allocator.make();
        d->set_bool(value);
        self->priv->add_member(name, d);
    }
}

void spino_docnode_add_member(SpinoDocNode* self, const gchar* name, SpinoDocNode* value)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        self->priv->add_member(name, value->priv);
        value->priv = Spino::dom_node_allocator.make();
    }
}

void spino_docnode_remove_member(SpinoDocNode* self, const gchar* name) 
{
    self->priv->remove_member(name);
}

void spino_docnode_append(SpinoDocNode* self, SpinoDocNode* other)
{
    self->priv->push_back(other->priv);
    other->priv = Spino::dom_node_allocator.make();
}


void spino_docnode_pop_back(SpinoDocNode* self)
{
    self->priv->pop_back();
}

void spino_docnode_remove(SpinoDocNode* self, SpinoValueIterator* iter)
{
    iter->iter = self->priv->erase(iter->iter);
}


SpinoDocView* spino_docnode_get_view(SpinoDocNode* self)
{
    assert(g_type_is_a(G_OBJECT_TYPE(self), SPINO_TYPE_DOCNODE));
    assert(self->priv != NULL);

    SpinoDocView* val = (SpinoDocView*)g_object_new(SPINO_TYPE_DOCVIEW, NULL);
    val->priv = self->priv;
    return val;
}


G_END_DECLS

