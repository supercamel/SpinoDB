#include "doc_object_private.h"
#include "doc_array_private.h"

G_BEGIN_DECLS

G_DEFINE_TYPE(SpinoDocObject, spino_docobject, G_TYPE_OBJECT)


SpinoDocObject* spino_docobject_new() 
{
    return (SpinoDocObject*)g_object_new(SPINO_TYPE_DOCOBJECT, NULL);
}

static void spino_docobject_finalize(GObject* object)
{
    SpinoDocObject* self = (SpinoDocObject*)object;
    self->doc->SetObject();
    G_OBJECT_CLASS(spino_docobject_parent_class)->finalize(object);
}

static void spino_docobject_class_init(SpinoDocObjectClass* klass)
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = spino_docobject_finalize;
}

static void spino_docobject_init(SpinoDocObject* self)
{
    self->doc = new Spino::DocType();
    self->doc->SetObject();
}

void spino_docobject_clear(SpinoDocObject* self)
{
    self->doc->SetObject();
}

void spino_docobject_add_int_member(SpinoDocObject* self, const gchar* name, const int value)
{
    Spino::ValueType v;
    v.SetInt(value);

    Spino::ValueType name_val(name, self->doc->GetAllocator());

    self->doc->AddMember(name_val, v, self->doc->GetAllocator());
}

void spino_docobject_add_uint_member(SpinoDocObject* self, const gchar* name, const guint value)
{
    Spino::ValueType v;
    v.SetUint(value);

    Spino::ValueType name_val(name, self->doc->GetAllocator());
    self->doc->AddMember(name_val, v, self->doc->GetAllocator());
}

void spino_docobject_add_double_member(SpinoDocObject* self, const gchar* name, const double value) 
{
    Spino::ValueType v;
    v.SetDouble(value);

    Spino::ValueType name_val(name, self->doc->GetAllocator());

    self->doc->AddMember(name_val, v, self->doc->GetAllocator());
}

void spino_docobject_add_string_member(SpinoDocObject* self, const gchar* name, const gchar* value) 
{
    Spino::ValueType v(value, self->doc->GetAllocator());
    Spino::ValueType name_val(name, self->doc->GetAllocator());
    self->doc->AddMember(name_val, v, self->doc->GetAllocator());
}


void spino_docobject_add_bool_member(SpinoDocObject* self, const gchar* name, const gboolean value)
{
    Spino::ValueType v;
    v.SetBool(value);

    Spino::ValueType name_val(name, self->doc->GetAllocator());
    self->doc->AddMember(name_val, v, self->doc->GetAllocator());
}

void spino_docobject_add_object_member(SpinoDocObject* self, const gchar* name, const SpinoDocObject* value)
{
    Spino::ValueType name_val(name, self->doc->GetAllocator());
    self->doc->AddMember(name_val, value->doc->GetObject(), self->doc->GetAllocator());
}

void spino_docobject_add_array_member(SpinoDocObject* self, const gchar* name, const SpinoDocArray* value)
{
    Spino::ValueType name_val(name, self->doc->GetAllocator());
    self->doc->AddMember(name_val, value->doc->GetArray(), self->doc->GetAllocator());
}

gboolean spino_docobject_has_member(SpinoDocObject* self, const gchar* name) 
{
    return self->doc->HasMember(name);
}

SpinoVALUETYPE spino_docobject_get_member_type(SpinoDocObject* self, const gchar* name) 
{
    if(self->doc->HasMember(name)) {
        if((*self->doc)[name].IsString()) {
            return SPINO_VALUETYPE_STRING;
        }
        else if((*self->doc)[name].IsBool()) {
            return SPINO_VALUETYPE_BOOL;
        }
        else if((*self->doc)[name].IsNumber()) {
            if((*self->doc)[name].IsInt()) {
                return SPINO_VALUETYPE_INT;
            }
            else if((*self->doc)[name].IsUint()) {
                return SPINO_VALUETYPE_UINT;
            }
            else {
                return SPINO_VALUETYPE_DOUBLE;
            }
        }
        else if((*self->doc)[name].IsArray()) {
            return SPINO_VALUETYPE_ARRAY;
        }
        else if((*self->doc)[name].IsObject()) {
            return SPINO_VALUETYPE_OBJECT;
        }
        else {
            return SPINO_VALUETYPE_NULL;
        }
    }
    else {
        return SPINO_VALUETYPE_UNDEFINED;
    }
}

int spino_docobject_get_int_member(SpinoDocObject* self, const gchar* name)
{
    return (*self->doc)[name].GetInt();
}

guint spino_docobject_get_uint_member(SpinoDocObject* self, const gchar* name)
{
    return (*self->doc)[name].GetUint();
}

gboolean spino_docobject_get_bool_member(SpinoDocObject* self, const gchar* name)
{
    return (*self->doc)[name].GetBool();
}

double spino_docobject_get_double_member(SpinoDocObject* self, const gchar* name)
{
    return (*self->doc)[name].GetDouble();
}

const gchar* spino_docobject_get_string_member(SpinoDocObject* self, const gchar* name)
{
    return (*self->doc)[name].GetString();
}

SpinoDocObject* spino_docobject_get_object_member(SpinoDocObject* self, const gchar* name) 
{
//    auto& v = (*self->doc)[name].GetObject();

}


gchar* spino_docobject_stringify(SpinoDocObject* self) 
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    self->doc->Accept(writer);
    return g_strdup(sb.GetString());
}


G_END_DECLS

