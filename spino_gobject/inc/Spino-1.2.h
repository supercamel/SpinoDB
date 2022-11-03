#ifndef SPINO_H_INCLUDED
#define SPINO_H_INCLUDED

#include "database.h"

G_BEGIN_DECLS

/**
 * spino_escape:
 * @str: a string to escape
 * Returns: (transfer full):
 */
gchar* spino_escape(const gchar* str);

/**
 * spino_unescape:
 * @str: a string to unescape
 * Returns: (transfer full):
 */
gchar* spino_unescape(const gchar* str);

G_END_DECLS

#endif

