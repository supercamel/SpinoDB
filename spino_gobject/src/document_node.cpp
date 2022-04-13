#include "document_node_private.h"
#include "document_viewer_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoDocNode, spino_docnode, G_TYPE_OBJECT)

static void spino_docnode_finalize(GObject* object) 
{
    SpinoDocNode* self = (SpinoDocNode*)(object);
    self->priv.SetObject();
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

SpinoDocNode* spino_docnode_new(rapidjson::CrtAllocator* a)
{
    SpinoDocNode* val = (SpinoDocNode*)g_object_new(SPINO_TYPE_DOCNODE, NULL);
    val->priv = Spino::ValueType();
    val->alloc = a;
    return val;
}


void spino_docnode_set_object(SpinoDocNode* self)
{
    self->priv.SetObject();
}

void spino_docnode_set_array(SpinoDocNode* self)
{
    self->priv.SetArray();
}

void spino_docnode_set_int(SpinoDocNode* self, int value)
{
    self->priv.SetInt(value);
}

void spino_docnode_set_uint(SpinoDocNode* self, guint value)
{
    self->priv.SetUint(value);
}

void spino_docnode_set_double(SpinoDocNode* self, double value) 
{
    self->priv.SetDouble(value);
}

void spino_docnode_set_bool(SpinoDocNode* self, gboolean value)
{
    self->priv.SetBool(value);
}

void spino_docnode_set_string(SpinoDocNode* self, const gchar* value)
{
    self->priv.SetString(value, strlen(value), *self->alloc);
}

void spino_docnode_set_null(SpinoDocNode* self)
{
    self->priv.SetNull();
}

void spino_docnode_add_int_member(SpinoDocNode* self, const gchar* name, const int value)
{
    if(self->priv.IsObject()) {
        Spino::ValueType v;
        v.SetInt(value);

        Spino::ValueType name_val(name, *self->alloc);
        self->priv.AddMember(name_val, v, *self->alloc);
    }
}

void spino_docnode_add_uint_member(SpinoDocNode* self, const gchar* name, const guint value)
{
    if(self->priv.IsObject()) {
        Spino::ValueType v;
        v.SetUint(value);

        Spino::ValueType name_val(name, *self->alloc);
        self->priv.AddMember(name_val, v, *self->alloc);
    }
}

void spino_docnode_add_double_member(SpinoDocNode* self, const gchar* name, const double value)
{
    if(self->priv.IsObject()) {
        Spino::ValueType v;
        v.SetDouble(value);

        Spino::ValueType name_val(name, *self->alloc);
        self->priv.AddMember(name_val, v, *self->alloc);
    }
}

void spino_docnode_add_string_member(SpinoDocNode* self, const gchar* name, const gchar* value)
{
    if(self->priv.IsObject()) {
        Spino::ValueType v(value, *self->alloc);
        Spino::ValueType name_val(name, *self->alloc);
        self->priv.AddMember(name_val, v, *self->alloc);
    }
}

void spino_docnode_add_bool_member(SpinoDocNode* self, const gchar* name, const gboolean value)
{
    if(self->priv.IsObject()) {
        Spino::ValueType v;
        v.SetBool(value);

        Spino::ValueType name_val(name, *self->alloc);
        self->priv.AddMember(name_val, v, *self->alloc);
    }
}

void spino_docnode_add_member(SpinoDocNode* self, const gchar* name, SpinoDocNode* value)
{
    if(self->priv.IsObject()) {
        Spino::ValueType name_val(name, *self->alloc);
        self->priv.AddMember(name_val, value->priv, *self->alloc);
    }
}

void spino_docnode_remove_member(SpinoDocNode* self, const gchar* name) 
{
    self->priv.RemoveMember(name);
}

void spino_docnode_append(SpinoDocNode* self, SpinoDocNode* other)
{
    self->priv.PushBack(other->priv, *self->alloc);
}


void spino_docnode_pop_back(SpinoDocNode* self)
{
    self->priv.PopBack();
}

void spino_docnode_remove(SpinoDocNode* self, SpinoValueIterator* iter)
{
    iter->iter = self->priv.Erase(iter->iter);
}


SpinoDocView* spino_docnode_get_view(SpinoDocNode* self)
{
    SpinoDocView* val = (SpinoDocView*)g_object_new(SPINO_TYPE_DOCVIEW, NULL);
    val->priv = &self->priv;
    return val;
}


G_END_DECLS

