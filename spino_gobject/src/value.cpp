#include "value_private.h"
#include "document_viewer_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoValue, spino_value, G_TYPE_OBJECT)

static void spino_value_finalize(GObject* object) 
{
    SpinoValue* self = (SpinoValue*)(object);
    self->priv.SetObject();
    G_OBJECT_CLASS(spino_value_parent_class)->finalize(object);
}

static void spino_value_class_init(SpinoValueClass* klass) 
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = spino_value_finalize;
}

static void spino_value_init(SpinoValue* self) 
{

}

SpinoValue* spino_value_new(rapidjson::CrtAllocator* a)
{
    SpinoValue* val = (SpinoValue*)g_object_new(SPINO_TYPE_VALUE, NULL);
    val->priv = Spino::ValueType();
    val->alloc = a;
    return val;
}


void spino_value_set_object(SpinoValue* self)
{
    self->priv.SetObject();
}

void spino_value_set_array(SpinoValue* self)
{
    self->priv.SetArray();
}

void spino_value_set_int(SpinoValue* self, int value)
{
    self->priv.SetInt(value);
}

void spino_value_set_uint(SpinoValue* self, guint value)
{
    self->priv.SetUint(value);
}

void spino_value_set_double(SpinoValue* self, double value) 
{
    self->priv.SetDouble(value);
}

void spino_value_set_bool(SpinoValue* self, gboolean value)
{
    self->priv.SetBool(value);
}

void spino_value_set_string(SpinoValue* self, const gchar* value)
{
    self->priv.SetString(value, strlen(value), *self->alloc);
}

void spino_value_set_null(SpinoValue* self)
{
    self->priv.SetNull();
}

void spino_value_add_int_member(SpinoValue* self, const gchar* name, const int value)
{
    if(self->priv.IsObject()) {
        Spino::ValueType v;
        v.SetInt(value);

        Spino::ValueType name_val(name, *self->alloc);
        self->priv.AddMember(name_val, v, *self->alloc);
    }
}

void spino_value_add_uint_member(SpinoValue* self, const gchar* name, const guint value)
{
    if(self->priv.IsObject()) {
        Spino::ValueType v;
        v.SetUint(value);

        Spino::ValueType name_val(name, *self->alloc);
        self->priv.AddMember(name_val, v, *self->alloc);
    }
}

void spino_value_add_double_member(SpinoValue* self, const gchar* name, const double value)
{
    if(self->priv.IsObject()) {
        Spino::ValueType v;
        v.SetDouble(value);

        Spino::ValueType name_val(name, *self->alloc);
        self->priv.AddMember(name_val, v, *self->alloc);
    }
}

void spino_value_add_string_member(SpinoValue* self, const gchar* name, const gchar* value)
{
    if(self->priv.IsObject()) {
        Spino::ValueType v(value, *self->alloc);
        Spino::ValueType name_val(name, *self->alloc);
        self->priv.AddMember(name_val, v, *self->alloc);
    }
}

void spino_value_add_bool_member(SpinoValue* self, const gchar* name, const gboolean value)
{
    if(self->priv.IsObject()) {
        Spino::ValueType v;
        v.SetBool(value);

        Spino::ValueType name_val(name, *self->alloc);
        self->priv.AddMember(name_val, v, *self->alloc);
    }
}

void spino_value_add_member(SpinoValue* self, const gchar* name, SpinoValue* value)
{
    if(self->priv.IsObject()) {
        Spino::ValueType name_val(name, *self->alloc);
        self->priv.AddMember(name_val, value->priv, *self->alloc);
    }
}

void spino_value_remove_member(SpinoValue* self, const gchar* name) 
{
    self->priv.RemoveMember(name);
}

void spino_value_append(SpinoValue* self, SpinoValue* other)
{
    self->priv.PushBack(other->priv, *self->alloc);
}


void spino_value_pop_back(SpinoValue* self)
{
    self->priv.PopBack();
}

void spino_value_remove(SpinoValue* self, SpinoValueIterator* iter)
{
    iter->iter = self->priv.Erase(iter->iter);
}


SpinoDocView* spino_value_get_viewer(SpinoValue* self)
{
    SpinoDocView* val = (SpinoDocView*)g_object_new(SPINO_TYPE_VALUE_ITERATOR, NULL);
    val->priv = &self->priv;
    return val;
}


G_END_DECLS

