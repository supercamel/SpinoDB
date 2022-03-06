#include "Spino-1.0.h"

#include "cppsrc/SpinoDB.h"


G_BEGIN_DECLS

gchar* spino_escape(const gchar* str)
{
    return g_strdup(Spino::escape(str).c_str());
}

G_END_DECLS

