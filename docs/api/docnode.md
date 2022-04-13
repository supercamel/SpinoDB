# DocNode

DocNodes are used to assemble new JSON documents directly in SpinoDBs memory. 

DocNodes are created using the Collection that the document is intended for (appending or updating).


```
    SpinoDocNode* doc = spino_collection_create_node(col);
```

## `spino_docnode_set_object(SpinoDocNode* self)`

Turns the DocNode into an Object and deletes any sub-nodes. 

**Parameters**

*self* The DocNode to turn into a JSON object

## `spino_docnode_set_array(SpinoDocNode* self)`

Turns the DocNode into an array and deletes any sub-nodes.

**Parameters**

*self* The DocNode to turn into a JSON array

## `spino_docnode_set_int(SpinoDocNode* self, int value)`

Turns the DocNode into type int with value v.

**Parameters**

*self* The DocNode to assign the int value to
*value* The value as an integer

