#include "document_viewer_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoMemberIterator, spino_member_iterator, G_TYPE_OBJECT)

static void spino_member_iterator_finalize(GObject* object)
{
    G_OBJECT_CLASS(spino_member_iterator_parent_class)->finalize(object);
}

static void spino_member_iterator_class_init(SpinoMemberIteratorClass* klass)
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = spino_member_iterator_finalize;
}

static void spino_member_iterator_init(SpinoMemberIterator* self)
{

}

SpinoMemberIterator* spino_member_iterator_new()
{
    SpinoMemberIterator* iter = (SpinoMemberIterator*)g_object_new(SPINO_TYPE_MEMBER_ITERATOR, NULL);
    return iter;
}


void spino_member_iterator_next(SpinoMemberIterator* iter)
{
    iter->iter++;
}

const gchar* spino_member_iterator_get_name(SpinoMemberIterator* iter)
{
    return iter->iter->name.GetString();
}

SpinoDocView* spino_member_iterator_get_view(SpinoMemberIterator* iter)
{
    SpinoDocView* val = (SpinoDocView*)g_object_new(SPINO_TYPE_DOCVIEW, NULL);
    val->priv = &(iter->iter->value);
    return val;
}

gboolean spino_member_iterator_compare(SpinoMemberIterator* iter, SpinoMemberIterator* other)
{
    return (iter->iter == other->iter);
}

gboolean spino_member_iterator_is_not(SpinoMemberIterator* iter, SpinoMemberIterator* other)
{
    return (iter->iter != other->iter);
}

G_DEFINE_TYPE(SpinoValueIterator, spino_value_iterator, G_TYPE_OBJECT)

static void spino_value_iterator_finalize(GObject* object)
{
    G_OBJECT_CLASS(spino_value_iterator_parent_class)->finalize(object);
}

static void spino_value_iterator_class_init(SpinoValueIteratorClass* klass)
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = spino_value_iterator_finalize;
}

static void spino_value_iterator_init(SpinoValueIterator* self)
{

}

SpinoValueIterator* spino_value_iterator_new()
{
    SpinoValueIterator* iter = (SpinoValueIterator*)g_object_new(SPINO_TYPE_VALUE_ITERATOR, NULL);
    return iter;
}


void spino_value_iterator_next(SpinoValueIterator* iter)
{
    iter->iter++;
}

SpinoDocView* spino_value_iterator_get_view(SpinoValueIterator* iter)
{
    SpinoDocView* val = (SpinoDocView*)g_object_new(SPINO_TYPE_DOCVIEW, NULL);
    val->priv = &(*iter->iter);
    return val;
}

gboolean spino_value_iterator_compare(SpinoValueIterator* iter, SpinoValueIterator* other)
{
    return (iter->iter == other->iter);
}

gboolean spino_value_iterator_is_not(SpinoValueIterator* iter, SpinoValueIterator* other)
{
    return (iter->iter != other->iter);
}

G_DEFINE_TYPE(SpinoDocView, spino_docview, G_TYPE_OBJECT)

static void spino_docview_finalize(GObject* object)
{
    G_OBJECT_CLASS(spino_docview_parent_class)->finalize(object);
}

static void spino_docview_class_init(SpinoDocViewClass* klass)
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = spino_docview_finalize;
}

static void spino_docview_init(SpinoDocView* self)
{

}


int spino_docview_get_int(SpinoDocView* self)
{
    return self->priv->GetInt();
}

guint spino_docview_get_uint(SpinoDocView* self)
{
    return self->priv->GetUint();
}

double spino_docview_get_double(SpinoDocView* self)
{
    return self->priv->GetDouble();
}

gboolean spino_docview_get_bool(SpinoDocView* self)
{
    return self->priv->GetBool();
}

const gchar* spino_docview_get_string(SpinoDocView* self)
{
    return self->priv->GetString();
}

gboolean spino_docview_has_member(SpinoDocView* self, const gchar* name)
{
    if(self->priv->IsObject()) {
        return self->priv->HasMember(name);
    }
    return false;
}

SpinoVALUETYPE spino_docview_get_value_type(SpinoDocView* self)
{
    if(self->priv->IsString()) {
        return SPINO_VALUETYPE_STRING;
    }
    else if(self->priv->IsBool()) {
        return SPINO_VALUETYPE_BOOL;
    }
    else if(self->priv->IsNumber()) {
        if(self->priv->IsInt()) {
            return SPINO_VALUETYPE_INT;
        }
        else if(self->priv->IsUint()) {
            return SPINO_VALUETYPE_UINT;
        }
        else {
            return SPINO_VALUETYPE_DOUBLE;
        }
    }
    else if(self->priv->IsArray()) {
        return SPINO_VALUETYPE_ARRAY;
    }
    else if(self->priv->IsObject()) {
        return SPINO_VALUETYPE_OBJECT;
    }
    else if(self->priv->IsNull()) {
        return SPINO_VALUETYPE_NULL;
    }
    else {
        return SPINO_VALUETYPE_UNDEFINED;
    }
}

SpinoVALUETYPE spino_docview_get_member_type(SpinoDocView* self, const gchar* name)
{
    if((*self->priv).HasMember(name)) {
        if((*self->priv)[name].IsString()) {
            return SPINO_VALUETYPE_STRING;
        }
        else if((*self->priv)[name].IsBool()) {
            return SPINO_VALUETYPE_BOOL;
        }
        else if((*self->priv)[name].IsNumber()) {
            if((*self->priv)[name].IsInt()) {
                return SPINO_VALUETYPE_INT;
            }
            else if((*self->priv)[name].IsUint()) {
                return SPINO_VALUETYPE_UINT;
            }
            else {
                return SPINO_VALUETYPE_DOUBLE;
            }
        }
        else if((*self->priv)[name].IsArray()) {
            return SPINO_VALUETYPE_ARRAY;
        }
        else if((*self->priv)[name].IsObject()) {
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

int spino_docview_get_int_member(SpinoDocView* self, const gchar* name)
{
    return (*self->priv)[name].GetInt();
}

guint spino_docview_get_uint_member(SpinoDocView* self, const gchar* name)
{
    return (*self->priv)[name].GetUint();
}

gboolean spino_docview_get_bool_member(SpinoDocView* self, const gchar* name)
{
    return (*self->priv)[name].GetBool();
}

double spino_docview_get_double_member(SpinoDocView* self, const gchar* name)
{
    return (*self->priv)[name].GetDouble();
}

const gchar* spino_docview_get_string_member(SpinoDocView* self, const gchar* name)
{
    return (*self->priv)[name].GetString();
}

SpinoDocView* spino_docview_get_member(SpinoDocView* self, const gchar* name)
{
    SpinoDocView* val = (SpinoDocView*)g_object_new(SPINO_TYPE_DOCVIEW, NULL);
    val->priv = &(*self->priv)[name];
    return val;
}

SpinoMemberIterator* spino_docview_member_begin(SpinoDocView* self)
{
    SpinoMemberIterator* ret = spino_member_iterator_new();
    ret->iter = self->priv->MemberBegin();
    return ret;
}

SpinoMemberIterator* spino_docview_member_end(SpinoDocView* self)
{
    SpinoMemberIterator* ret = spino_member_iterator_new();
    ret->iter = self->priv->MemberEnd();
    return ret;
}

SpinoDocView* spino_docview_get_element(SpinoDocView* self, guint index)
{
    SpinoDocView* val = (SpinoDocView*)g_object_new(SPINO_TYPE_DOCVIEW, NULL);
    val->priv = &(*self->priv)[index];
    return val;
}

SpinoValueIterator* spino_docview_begin(SpinoDocView* self)
{
    SpinoValueIterator* ret = spino_value_iterator_new();
    ret->iter = self->priv->Begin();
    return ret;
}

SpinoValueIterator* spino_docview_end(SpinoDocView* self)
{
    SpinoValueIterator* ret = spino_value_iterator_new();
    ret->iter = self->priv->End();
    return ret;
}

gchar* spino_docview_stringify(SpinoDocView* self)
{
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    self->priv->Accept(writer);
    return g_strdup(sb.GetString());

}


G_END_DECLS

