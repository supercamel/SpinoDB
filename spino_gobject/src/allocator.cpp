#include "allocator_private.h"

G_BEGIN_DECLS


G_DEFINE_TYPE(SpinoAllocator, spino_allocator, G_TYPE_OBJECT)

static void spino_allocator_class_init(SpinoAllocatorClass* klass) 
{

}

static void spino_allocator_init(SpinoAllocator* self) 
{

}

SpinoAllocator* spino_allocator_new(rapidjson::CrtAllocator* alloc)
{
    SpinoAllocator* a = (SpinoAllocator*)g_object_new(SPINO_TYPE_ALLOCATOR, NULL);
    a->priv = alloc;
    return a;
}

G_END_DECLS

