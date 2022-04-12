#ifndef GSPINO_ALLOCATOR_H_INCLUDED
#define GSPINO_ALLOCATOR_H_INCLUDED

#include <glib-object.h>

G_BEGIN_DECLS

#define SPINO_TYPE_ALLOCATOR (spino_allocator_get_type())

G_DECLARE_FINAL_TYPE(SpinoAllocator, spino_allocator, Spino, Allocator, GObject)



G_END_DECLS

#endif


