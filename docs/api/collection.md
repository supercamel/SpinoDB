# Collection

A collection is a group of documents. Collections are returned by the Database.getCollection() function. 


## getName

getName returns the name of the collection. 

**Parameters** 

None

**Returns** 

The name of the collection as a string.


## createIndex

createIndex takes a field name and uses it to order the data by the value of the field. This makes it possible for SpinoDB to search the collection much more efficiently, because it's far easier to search through ordered data than unordered data. 

Collections can have multiple indexes.

Always create indexes for fields that are used to identify documents. 

    createIndex(<field_name>)

Note that indexes are not persisted to disk and must be created each time the database is initialised.

**Parameters**

1. The field name. Sub-object fields can also be specified using a '.' character. e.g. 'profile.number' 

**Returns**

No return value

## dropIndex

dropIndex deletes an index. Dropping an index will causes queries that use the field to become slower but will free up some memory. 

    dropIndex(<field_name>)

**Parameters**

1. The index field name.

**Returns**

No return value

## append

append will add a document to the collection. The document must be a valid JSON string. NodeJS bindings will also accept javascript objects. 

    append(<json_string>)

**Parameters**

1. A valid JSON object string to append to the collection.

**Returns**

No return value

## update

update will merge a document into existing documents that match the query. update can only add or modify existing fields. It does not delete fields. 

    update(<query>, <document>);

**Parameters**

1. A query string
2. A JSON document to merge

**Returns**

No return value.

## upsert

The upsert function is basically shorthand for drop + insert. It will remove any document that matches the query and then append a new document to the collection.

    upsert(<query>, <document>);

**Parameters**

1. A query string
2. A JSON document to merge

**Returns**

No return value.

## findOne

findOne will return the first document that matches the query string, or an empty string if no documents match the query. 

    findOne(<query>)

**Parameters**

1. A query as a string

**Returns**

A matching JSON document, or an empty string if no matching document is found.

## find

find will run a query and return a Cursor that can be used to iterate over the results. 

    find(<query>)

**Parameters**

1. A query as a string

**Returns**

A Cursor that can be used to iterate the results. Refer to the Cursor API documentation. 

## drop

Deletes every document that matches the query.

    drop(<query>, <limit>)

**Parameters**

1. A query as a string
2. Optional. A maximum number of documents to remove. By default, there is no limit.

**Returns**

The number of documents that were deleted.

## size

size returns the number of documents in the collection.

    size()

**Parameters** 

No parameters

**Returns**

The number of documents in the collection.



