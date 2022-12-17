# Cursor

Cursors are returned by calls to Collection.find() and are used to iterate the results of a query.

Cursors can also apply projections to return only selected fields and can run Squirrel scripts to process query results. 
 

## hasNext

hasNext checks if the cursor has any more documents remaining. 

**Parameters**

None

**Returns** 

True if there are documents remaining, false if not

## next

next gets the next document as a JSON string

**Parameters**

None

**Returns**

The next query result as a JSON string

**Example**

NodeJS

```
while(cursor.hasNext()) {
    var doc = cursor.next();
    // do something with the document
}
```

## count

Returns the number of documents that the cursor will retrieve. Note that this will iterate the cursor and can be a (relatively) slow function to call. 

**Parameters**

None

**Returns**

The number of documents that the cursor will retrieve.

## setLimit

Limits the number of documents that the cursor can retrieve.

**Parameters**

1. A maximum number of documents to retrieve before hasNext begins to return false.

**Returns** 

A self-reference to the cursor.

## runScript

*DEPRECATED* don't use, please plan to remove it from existing code.

Cursors can run Squirrel scripts to order, aggregate, format or otherwise process query results. 

Refer to the scripting section for more details.


**Parameters**

1. A squirrel script as a string

**Returns**

The result of the script as a string

