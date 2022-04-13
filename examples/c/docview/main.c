#include <stdio.h>
#include <Spino-1.0.h>


void add_entries(SpinoCollection* col);


int main() {
    SpinoDatabase* db = spino_database_new();

    SpinoCollection* col = spino_database_add_collection(db, "my_collection");
    spino_collection_create_index(col, "idx");

    /*
    * The JSON document schema is like this
    *
    * {
    *    "idx": number,
    *    "arr": [ number, number ... ]
    * }
    *
     */
    add_entries(col);

    // query for the document with idx: 5 and get a view of it
    // note this view is a read-only peek at the document inside the collection DOM
    // there is no copying of nodes or stringification here
    SpinoDocView* view = spino_collection_find_one_view(col, "{idx: 5}");
    
    /*
     * SpinoMemberIterator is used to traverse the members of a JSON object
     */
    SpinoMemberIterator* miter = spino_docview_member_begin(view);
    while(spino_member_iterator_is_not(miter, spino_docview_member_end(view))) {
        // spino_member_iterator_get_name() gets the key name of the field that the iter points to
        const char* name = spino_member_iterator_get_name(miter);
        printf("name: %s\n", name);

        //' do a check to make sure the value of idx is infact 5
        if(strncmp(name, "idx", 4) == 0) {
            SpinoDocView* idx_member = spino_member_iterator_get_view(miter);
            int idx_val = spino_docview_get_int(idx_member);
            if(idx_val != 5) {
                printf("**flushed** the query was to match idx of 5. how embarrasing.\n");
            }
        }
        
        //get next member
        spino_member_iterator_next(miter);
    }


    /**
     * Get the arr member, which is an array of numbers
     */
    SpinoDocView* arr = spino_docview_get_member(view, "arr");
    // lil check to ensure it is, infact, an array
    if(spino_docview_get_value_type(arr) == SPINO_VALUETYPE_ARRAY) 
    {
        // use a SpinoValueIterator to iterate along an array
        // SpinoMemberIterator is used to iterate an object, SpinoValueIterator is for arrays
        SpinoValueIterator* viter = spino_docview_begin(arr);
        // while there are nodes left to view in the array 
        while(spino_value_iterator_is_not(viter, spino_docview_end(arr))) {
            // get a view of the current iter node
            SpinoDocView* vview = spino_value_iterator_get_view(viter);
            // print the value which is known to be an int
            // though, if you didn't know you could use spino_docview_get_value_type() to check
            printf("%i ", spino_docview_get_int(vview));

            // increment the iter
            spino_value_iterator_next(viter);
        }
        printf("\n");
    }

    g_object_unref(view);
    g_object_unref(db);

    return 0;
}


void add_entries(SpinoCollection* col) 
{
    SpinoDocNode* doc = spino_collection_create_node(col);
    SpinoDocNode* arr = spino_collection_create_node(col);
    SpinoDocNode* inode = spino_collection_create_node(col);

    for(int i = 0; i < 100; i++) {
        spino_docnode_set_object(doc);
        spino_docnode_add_int_member(doc, "idx", i);
        spino_docnode_set_array(arr);

        for(int j = 0; j < 5; j++) {
            spino_docnode_set_int(inode, j);
            spino_docnode_append(arr, inode);
        }

        spino_docnode_add_member(doc, "arr", arr);
        spino_collection_append_node(col, doc);
    }

    g_object_unref(doc);
    g_object_unref(arr);
    g_object_unref(inode);
}
