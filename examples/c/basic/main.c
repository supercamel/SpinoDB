#include <stdio.h>
#include <Spino-1.2.h>

int main() {
    SpinoDatabase* db = spino_database_new();

    SpinoCollection* col = spino_database_get_collection(db, "my_collection");
    spino_collection_create_index(col, "idx");

    for(int i = 0; i < 10; i++) {
        GString* s = g_string_new(NULL);
        g_string_printf(s, "{\"idx\": %i}", i);

        spino_collection_append(col, s->str);

        g_string_free(s, TRUE);
    }
    
    SpinoCursor* cur = spino_collection_find(col, "{idx: {$gt: 5}}");
    while(spino_cursor_has_next(cur) == TRUE) {
        printf("%s\n", spino_cursor_next(cur));
    }

    spino_database_save(db, "db.json");

    g_object_unref(cur);
    g_object_unref(col);
    g_object_unref(db);
    return 0;
}

