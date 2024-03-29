# DocView & DocNode 

DocView and DocNode are GObjects provided in the GObject bindings for creating and reading JSON data. They are not available in NodeJS because Javascript is already highly optimised for processing JSON. 

SpinoDB relies on rapidjson heavily for json parsing, serialisation and the DOM. DocView and DocNode are lightweight wrappers around some rapidjson DOM functions. 


## DocView Description

A DocView is a 'read-only' peek at a value inside SpinoDBs DOM. DocViews do not copy memory or do any string processing, and are a very efficient way to get information out of the document. DocView is also quite a convenient and simple API for traversing the JSON DOM in C / Vala. 

DocViews are created by cursors with the spino_cursor_next_view() function, and by collections with spino_collection_find_one_view()

```
    SpinoDocView* view = spino_cursor_next_view(cursor);
    // OR
    SpinoDocView* view = spino_collection_find_one_view(col);
```

The view represents a JSON value. It could be a numeric value, an object, an array, string, etc, however spino_cursor_next_view() is gauranteed to return an object because documents are objects. 

Members of the object can be read directly using these functions

```
    int spino_docview_get_int_member(SpinoDocView* docview, const gchar* name);
    guint spino_docview_get_uint_member(SpinoDocView* docview, const gchar* name);
    gboolean spino_docview_get_bool_member(SpinoDocView* docview, const gchar* name);
    double spino_docview_get_double_member(SpinoDocView* docview, const gchar* name);
    const gchar* spino_docview_get_string_member(SpinoDocView* docview, const gchar* name);
```

Sub-object and array members must projected into a DocView of their own using spino_docview_get_member()
```
SpinoDocView* spino_docview_get_member(SpinoDocView* self, const gchar* name);
```

## MemberIterators

Objects can be traversed using MemberIterators. This example prints every field name in the object.

```
    SpinoMemberIterator* iter = spino_docview_member_begin(docview);
    while(spino_member_iterator_is_not(iter, spino_docview_member_end(docview)) == False) {
        printf("%s\n", spino_member_iterator_get_name(iter));
        spino_member_iterator_next(iter);
    }
```

The value of an iter can be obtained by getting a DocView of the iter contents.

```
    SpinoDocView* memberview = spino_member_iterator_get_view(iter);

    if(spino_docview_get_value_type(memberview) == SPINO_VALUETYPE_INT) {
        int value = spino_docview_get_int(memberview);
    }
```


## ValueIterators

Arrays are traversed using ValueIterators. ValueIterators have the same API as MemberIterators, only they do not have a 'get_name' function.

```
    if(spino_docview_get_value_type(docview) == SPINO_VALUETYPE_ARRAY) {
        SpinoValueIterator* iter = spino_docview_begin(docview);
        while(spino_value_iterator_is_not(iter, spino_docview_end(docview)) == False) {
            SpinoDocView* memberview = spino_value_iterator_get_view(iter);
            // extract value of memberview here

            spino_value_iterator_next(iter);
        }
    }
```

## DocNode Description

DocNode is a type for building JSON documents. 

To create a DocNode, call spino_docnode_new().

```
    SpinoDocNode* doc = spino_docnode_new();
```

A DocNode can contain any JSON value, such as an object, array, integer, string, etc. When it is created, it will contain type NULL. All documents must be of type object, so the root node of a document must be an object. After creating the base DocNode for a new document, call the `set_object()`function. 

```
    spino_docnode_set_object(doc);
```

Once the node is an object, members can be added. 

```
    spino_docnode_add_int_member(doc, "idx", 1);
    spino_docnode_add_string_member(doc, "name", "Dave");
```

Arrays can be created and added to the document

```
    SpinoDocNode* arr = spino_docnode_new();
    spino_docnode_set_array(arr); //set the node to be an array

    // create a node to temporarily hold values that we append to the array
    SpinoDocNode* element = spino_docnode_new();

    // set_string sets the node type to be type string with value "Hello"
    spino_docnode_set_string(arr, element, "Hello"); 
    spino_docnode_append(arr, element);

    spino_docnode_set_int(element, 100);
    spino_docnode_append(arr, element);

    // now the array looks like this 
    // ["Hello", 100]

    // add the array to the document
    spino_docnode_add_member(doc, "arr", arr);

    g_object_unref(element); // DocNodes must always been unreffed when they are no longer needed
    g_object_unref(arr);

```

## Move Semantics

When a DocNode is moved, the DOM elements are transferred to the new DocNode container and the previous DocNode becomes type NULL. For example


```
    // arr contains ["Hello", 100]
    // add arr to the document
    spino_docnode_add_member(doc, "myArray", arr);

    // 'arr' is now empty / null
    // doc now looks like this 
    // {myArray: ["Hello", 100]}
```

Note that every DocNode must be unreffed when it is no longer needed, even if it is moved into another DocNode. 

```
    g_object_unref(arr);
```

## Viewing DocNodes

DocNodes are used to create new documents, to view them create a DocView that can traverse and stringify the DOM. 

```
    DocView* view = spino_docnode_get_view(doc);
    printf("%s\n", spino_docview_stringify(view));
```

