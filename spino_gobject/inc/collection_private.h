#ifndef SPINO_COLLECTION_PRIVATE_H
#define SPINO_COLLECTION_PRIVATE_H

#include "collection.h"
#include "cppsrc/collection.h"

G_BEGIN_DECLS


struct _SpinoCollection{
    GObject parent_instance;
    Spino::Collection* priv;
};

SpinoCollection* spino_collection_new(Spino::Collection* c);

G_END_DECLS

#endif


