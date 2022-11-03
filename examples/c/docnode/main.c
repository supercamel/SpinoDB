#include <stdio.h>
#include <Spino-1.0.h>
#include <time.h>

void add_entries(SpinoCollection* col) 
{
    // pre-make some node objects
    SpinoDocNode* doc = spino_collection_create_node(col);
    SpinoDocNode* arr = spino_collection_create_node(col);
    SpinoDocNode* str = spino_collection_create_node(col);
    SpinoDocNode* dnode = spino_collection_create_node(col);

    for(int i = 0; i < 1000000; i++) {
        // set the document root node to be an object
        // this is mandatory. 
        // nodes to be added to the collection must be objects
        spino_docnode_set_object(doc);

        // add field "idx" with an integer value of i
        spino_docnode_add_int_member(doc, "idx", i);

        // add an array field
        // set the node to be an empty array
        spino_docnode_set_array(arr);

        // add some things to the array just for fun
        spino_docnode_set_string(str, "Hello world");
        spino_docnode_set_double(dnode, 153.023);
         
        // move the nodes into the array, note that this is a move and not a copy
        spino_docnode_append(arr, str);
        spino_docnode_append(arr, dnode);
        // 'str' and 'dnode' are now of type 'null'
        

        // now add the array to the document
        spino_docnode_add_member(doc, "arr", arr);

        // move the node to the collection
        // this removes the nodes from 'doc' and transfers them into the collection
        // doing so avoids a deep copy and needless memory allocations
        //spino_collection_append_node(col, doc);
        // 'doc' is now of type 'null'
    }

    // release the node objects
    g_object_unref(doc);
    g_object_unref(arr);
    g_object_unref(str);
    g_object_unref(dnode);
}


int main() {
    SpinoDatabase* db = spino_database_new();

    SpinoCollection* col = spino_database_add_collection(db, "my_collection");
    spino_collection_create_index(col, "idx");

    clock_t start, end;
    start = clock();
    add_entries(col);
    end = clock();
    double cpu_time = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("time: %f\n", cpu_time);

    SpinoCursor* cur = spino_collection_find(col, "{idx: {$gt: 5}}");
    while(spino_cursor_has_next(cur) == TRUE) {
        spino_cursor_next(cur);
    }

    g_object_unref(cur);
    g_object_unref(db);

    return 0;
}

