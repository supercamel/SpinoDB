# DocNode

DocNodes are used to assemble new JSON documents directly in SpinoDBs memory. DocNode does not exist in NodeJS bindings. It is only in GObject bindings.

To create a DocNode

```
    SpinoDocNode* doc = spino_docnode_new();
```

## set_object

    `spino_docnode_set_object(SpinoDocNode* self)`

Turns the DocNode into an Object and deletes any sub-nodes. 

## set_array

    `spino_docnode_set_array(SpinoDocNode* self)`

Turns the DocNode into an array and deletes any sub-nodes.

## set_int

    `spino_docnode_set_int(SpinoDocNode* self, int value)`

Turns the DocNode into an integer node and assigns the value to it

## set_uint

    `spino_docnode_set_uint(SpinoDocNode* self, guint value)`

Turns the DocNode into an unsigned int node and assigns the value to it

## set_double

    `spino_docnode_set_double(SpinoDocNode* self, double value)`

Turns the DocNode into a double node and assigns the value to it

## set_bool

    `spino_docnode_set_bool(SpinoDocNode* self, gboolean value)`

Turns the DocNode into a bool with value 'value'

## set_string

    `spino_docnode_set_string(SpinoDocNode* self, const gchar* value)`

Turns the DocNode into a string. The string is deep copied into the DocNode DOM.

## set_null

    `spino_docnode_set_null(SpinoDocNode* self)`

Turns the DocNode into a null type

## add_int_member

    `spino_docnode_add_int_member(SpinoDocNode* self, const gchar* name, const int value)`

If 'self' is an object, an integer field is added to the object with name as 'name' and value 'value'.

Example
```
    spino_docnode_set_object(doc); // make the node an empty object
    spino_docnode_add_int_member(doc, "idx", 1);

    // doc now looks like this JSON string
    // { idx: 1 }
```

## add_uint_member

    `spino_docnode_add_uint_member(SpinoDocNode* self, const gchar* name, const guint value)`

If 'self' is an object, a uint field is added to the object with name as 'name' and value 'value'.

## add_double_member

    `spino_docnode_add_double_member(SpinoDocNode* self, const gchar* name, const double value)`

If 'self' is an object, a double field is added to the object with name as 'name' and value 'value'.

## add_string_member

    `spino_docnode_add_string_member(SpinoDocNode* self, const gchar* name, const gchar* value)`

If 'self' is an object, a string field is added to the object with name as 'name' and value 'value'.

## add_bool_member

    `spino_docnode_add_bool_member(SpinoDocNode* self, const gchar* name, const gboolean value)`

If 'self' is an object, a bool field is added to the object with name as 'name' and value 'value'.

## add_member

    `spino_docnode_add_member(SpinoDocNode* self, const gchar* name, SpinoDocNode* value)`

If 'self' is an object, a new member is created with name as 'name'. 'value' is moved into 'self' as the value. Afterwards, 'value' will be of type null and 'self' will contain the nodes that 'value' originally contained. 

## remove_member

    `spino_docnode_remove_member(SpinoDocNode* self, const gchar* name)`

If 'self' is an object, a member of name 'name' will be deleted.

## append

    `spino_docnode_append(SpinoDocNode* self, SpinoDocNode* other)`

This function is used to append values to an array. 'self' must be of type array. The nodes contained by 'other' will be moved to the array contained in 'self' and afterwards 'other' will be of type null.

## pop_back

    `spino_docnode_pop_back(SpinoDocNode* self)`

If 'self' is of type array, the last element of the array will be deleted.

## remove

    `spino_docnode_remove(SpinoDocNode* self, SpinoValueIterator* iter)`

Removes an element from an array by an iterator. To get the iterator use spino_docnode_get_view() to first get a view, then use spino_view_begin() to get a SpinoValueIterator. Call spino_value_iterator_next() until the iterator points at the element you wish to remove. Then provide the iterator to this function to delete it. 

## get_view

    `spino_docnode_get_view(SpinoDocNode* self)`

Returns a SpinoDocView object that can traverse and inspect the 'self'

