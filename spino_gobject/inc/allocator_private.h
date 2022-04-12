#ifndef SPINO_ALLOCATOR_PRIVATE_H
#define SPINO_ALLOCATOR_PRIVATE_H

#include "allocator.h"
#include "cppsrc/SpinoDB.h"

G_BEGIN_DECLS

struct _SpinoAllocator {
    GObject parent_instance;
    rapidjson::CrtAllocator* priv;
};

SpinoAllocator* spino_allocator_new(rapidjson::CrtAllocator* priv);

G_END_DECLS

#endif


