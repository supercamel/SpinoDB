#include "doc_array_private.h"
#include "doc_object_private.h"

G_BEGIN_DECLS

G_DEFINE_TYPE(SpinoDocArray, spino_docarray, G_TYPE_OBJECT)


SpinoDocArray* spino_docarray_new() 
{
    return (SpinoDocArray*)g_object_new(SPINO_TYPE_DOCARRAY, NULL);
}

static void spino_docarray_finalize(GObject* object)
{
    SpinoDocArray* self = (SpinoDocArray*)object;
    self->doc->SetArray();
    G_OBJECT_CLASS(spino_docarray_parent_class)->finalize(object);
}

static void spino_docarray_class_init(SpinoDocArrayClass* klass)
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = spino_docarray_finalize;
}

static void spino_docarray_init(SpinoDocArray* self)
{
    self->doc = new Spino::DocType();
    self->doc->SetArray();
}

void spino_docarray_clear(SpinoDocArray* self)
{
    self->doc->SetArray();
}

void spino_docarray_add_int_element(SpinoDocArray* self, const int value)
{
    Spino::ValueType v;
    v.SetInt(value);
    self->doc->PushBack(v, self->doc->GetAllocator());
}

void spino_docarray_add_uint_element(SpinoDocArray* self, const guint value)
{
    Spino::ValueType v;
    v.SetUint(value);
    self->doc->PushBack(v, self->doc->GetAllocator());
}

void spino_docarray_add_double_element(SpinoDocArray* self, const double value) 
{
    Spino::ValueType v;
    v.SetDouble(value);
    self->doc->PushBack(v, self->doc->GetAllocator());
}

void spino_docarray_add_string_element(SpinoDocArray* self, const gchar* value) 
{
    Spino::ValueType v(value, self->doc->GetAllocator());
    self->doc->PushBack(v, self->doc->GetAllocator());
}


void spino_docarray_add_bool_element(SpinoDocArray* self, const gboolean value)
{
    Spino::ValueType v;
    v.SetBool(value);
    self->doc->PushBack(v, self->doc->GetAllocator());
}

void spino_docarray_add_object_element(SpinoDocArray* self, SpinoDocObject* value)
{
    self->doc->PushBack(value->doc->GetObject(), self->doc->GetAllocator());
}

void spino_docarray_add_array_element(SpinoDocArray* self, SpinoDocArray* value)
{
    self->doc->PushBack(value->doc->GetArray(), self->doc->GetAllocator());
}

gchar* spino_docarray_stringify(SpinoDocArray* self) 
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    self->doc->Accept(writer);
    return g_strdup(sb.GetString());
}


G_END_DECLS

