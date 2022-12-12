
#include "../inc/document_viewer_private.h"

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
    return g_strdup(iter->iter.get_key());
}

SpinoDocView* spino_member_iterator_get_view(SpinoMemberIterator* iter)
{
    SpinoDocView* val = (SpinoDocView*)g_object_new(SPINO_TYPE_DOCVIEW, NULL);
    val->priv = &(iter->iter.get_value());
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


gint64 spino_docview_get_int(SpinoDocView* self)
{
    return self->priv->get_int();
}

guint64 spino_docview_get_uint(SpinoDocView* self)
{
    return self->priv->get_uint();
}

double spino_docview_get_double(SpinoDocView* self)
{
    return self->priv->get_double();
}

gboolean spino_docview_get_bool(SpinoDocView* self)
{
    return self->priv->get_bool();
}

const gchar* spino_docview_get_string(SpinoDocView* self)
{
    return g_strdup(self->priv->get_string());
}

gboolean spino_docview_has_member(SpinoDocView* self, const gchar* name)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        return self->priv->has_member(name);
    }
    return false;
}

SpinoVALUETYPE spino_docview_get_value_type(SpinoDocView* self)
{
    switch(self->priv->get_type()) {
        case Spino::DOM_NODE_TYPE_INT:
            return SPINO_VALUETYPE_INT;
        case Spino::DOM_NODE_TYPE_UINT:
            return SPINO_VALUETYPE_UINT;
        case Spino::DOM_NODE_TYPE_DOUBLE:
            return SPINO_VALUETYPE_DOUBLE;
        case Spino::DOM_NODE_TYPE_BOOL:
            return SPINO_VALUETYPE_BOOL;
        case Spino::DOM_NODE_TYPE_LONG_STRING:
            return SPINO_VALUETYPE_STRING;
        case Spino::DOM_NODE_TYPE_SHORT_STRING:
            return SPINO_VALUETYPE_STRING;
        case Spino::DOM_NODE_TYPE_ARRAY:
            return SPINO_VALUETYPE_ARRAY;
        case Spino::DOM_NODE_TYPE_OBJECT:
            return SPINO_VALUETYPE_OBJECT;
        case Spino::DOM_NODE_TYPE_NULL:
            return SPINO_VALUETYPE_NULL;
        default:
            return SPINO_VALUETYPE_UNDEFINED;
    }
}

SpinoVALUETYPE spino_docview_get_member_type(SpinoDocView* self, const gchar* name)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        if(self->priv->has_member(name)) {
            auto member = self->priv->get_member(name);
            switch(member.get_type()) {
                case Spino::DOM_NODE_TYPE_INT:
                    return SPINO_VALUETYPE_INT;
                case Spino::DOM_NODE_TYPE_UINT:
                    return SPINO_VALUETYPE_UINT;
                case Spino::DOM_NODE_TYPE_DOUBLE:
                    return SPINO_VALUETYPE_DOUBLE;
                case Spino::DOM_NODE_TYPE_BOOL:
                    return SPINO_VALUETYPE_BOOL;
                case Spino::DOM_NODE_TYPE_LONG_STRING:
                    return SPINO_VALUETYPE_STRING;
                case Spino::DOM_NODE_TYPE_SHORT_STRING:
                    return SPINO_VALUETYPE_STRING;
                case Spino::DOM_NODE_TYPE_ARRAY:
                    return SPINO_VALUETYPE_ARRAY;
                case Spino::DOM_NODE_TYPE_OBJECT:
                    return SPINO_VALUETYPE_OBJECT;
                case Spino::DOM_NODE_TYPE_NULL:
                    return SPINO_VALUETYPE_NULL;
                default:
                    return SPINO_VALUETYPE_UNDEFINED;
            }
        }
    }
    return SPINO_VALUETYPE_UNDEFINED;
}

int spino_docview_get_int_member(SpinoDocView* self, const gchar* name)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        if(self->priv->has_member(name)) {
            auto member = self->priv->get_member(name);
            if(member.get_type() == Spino::DOM_NODE_TYPE_INT) {
                return member.get_int();
            }
        }
    }
    return -1;
}

guint spino_docview_get_uint_member(SpinoDocView* self, const gchar* name)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        if(self->priv->has_member(name)) {
            auto member = self->priv->get_member(name);
            if(member.get_type() == Spino::DOM_NODE_TYPE_UINT) {
                return member.get_uint();
            }
        }
    }
    return 0;
}

gboolean spino_docview_get_bool_member(SpinoDocView* self, const gchar* name)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        if(self->priv->has_member(name)) {
            auto member = self->priv->get_member(name);
            if(member.get_type() == Spino::DOM_NODE_TYPE_BOOL) {
                return member.get_bool();
            }
        }
    }
    return false;
}

double spino_docview_get_double_member(SpinoDocView* self, const gchar* name)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        if(self->priv->has_member(name)) {
            auto member = self->priv->get_member(name);
            if(member.get_type() == Spino::DOM_NODE_TYPE_DOUBLE) {
                return member.get_double();
            }
        }
    }
    return 0.0;
}

const gchar* spino_docview_get_string_member(SpinoDocView* self, const gchar* name)
{
    if(self->priv->get_type() == Spino::DOM_NODE_TYPE_OBJECT) {
        if(self->priv->has_member(name)) {
            auto member = self->priv->get_member(name);
            if(member.is_string()) {
                return g_strdup(member.get_string());
            }
        }
    }
    return NULL;
}

SpinoDocView* spino_docview_get_member(SpinoDocView* self, const gchar* name)
{
    SpinoDocView* val = (SpinoDocView*)g_object_new(SPINO_TYPE_DOCVIEW, NULL);
    val->priv = &self->priv->get_member(name);
    return val;
}

SpinoMemberIterator* spino_docview_member_begin(SpinoDocView* self)
{
    SpinoMemberIterator* ret = spino_member_iterator_new();
    ret->iter = self->priv->member_begin();
    return ret;
}

SpinoMemberIterator* spino_docview_member_end(SpinoDocView* self)
{
    SpinoMemberIterator* ret = spino_member_iterator_new();
    ret->iter = self->priv->member_end();
    return ret;
}

SpinoDocView* spino_docview_get_element(SpinoDocView* self, guint index)
{
    SpinoDocView* val = (SpinoDocView*)g_object_new(SPINO_TYPE_DOCVIEW, NULL);
    val->priv = &self->priv->get_element(index);
    return val;
}

SpinoValueIterator* spino_docview_begin(SpinoDocView* self)
{
    SpinoValueIterator* ret = spino_value_iterator_new();
    ret->iter = self->priv->element_begin();
    return ret;
}

SpinoValueIterator* spino_docview_end(SpinoDocView* self)
{
    SpinoValueIterator* ret = spino_value_iterator_new();
    ret->iter = self->priv->element_end();
    return ret;
}

gboolean spino_docview_compare(SpinoDocView* self, SpinoDocView* other)
{
    return *self->priv == *other->priv;
}

gchar* spino_docview_stringify(SpinoDocView* self)
{
    return g_strdup(self->priv->stringify().c_str());

}


G_END_DECLS
