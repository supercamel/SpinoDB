# Overview 

With Spino, you can create databases. Databases contain Collections. Collections contain JSON formatted documents.

Collections can be searched to find specific documents using the Spino query language (definitely not called SQL for short). Documents can be retrieved, updated, removed and new documents can be appended to the Collection. 

The JSON documents are not required to conform to an schema. Each document in a collection can be completely unique and different. However, it is good practice to have at least one field that is consistent in every document so the documents can be indexed. Indexing is extremely important for query performance. 

SpinoDB is designed for fast retrieval of documents even when the documents number in the millions. Spino can insert over a million simple documents per second and perform a find query in under 0.1ms. That means it can search 1 million documents 10k times per second. In reality, it doesn't matter how many documents are in the database, the only real limit is how much RAM you have available, and loading/saving time.  

## Creating A Database

```
// create a new database object
var db = new spino.Spino();

// load existing data from a file
db.load('data.db');

// optionally, enable journaling and consolidate the journal file
// every change to the data will now be recorded to the journal file
// consolidate() loads the journal file and 'replays' it against the data, then saves the data to file
// following this process at initialisation will ensure data is not lost if something bad happens between saves
db.enableJournal('journal');
db.consolidate('data.db');

```


## Creating Collections

```
// get the 'myCollection' collection
// if it doesn't exist, it is created
var col = db.getCollection("myCollection");

// index the documents in this collection by the idx field
// this means future find queries that use this field will execute much faster 
// usually its best to do this after loaded the database
col.createIndex("idx");

```

## Inserting Documents

```
col.append({ idx: 1, name: "Dave" });
```

## Finding Documents

If only one result is expected, the findOne function should be used.

```
var docstr = col.findOne("{idx:1}");
// docstr now contains the JSON document as a string
```

For iterating over multiple results, use the find function. find will return a Cursor object which is used to iterate over the results. 

```
var cursor = col.find("{}"); // empty query to return all documents
while(cursor.hasNext()) {
    console.log(cursor.next());
}
```

## Updating Documents

The collection update function will merge a new document into an existing one. It cannot remove fields but it will add new fields and overwrite existing ones.

```
// for all documents where idx is 1, change the name field to Steve
col.update("{idx:1}", {name:"Steve"});
```

## Deleting Documents

```
// delete all documents where idx is 1
col.drop("{idx:1"});
```

## Key Value Store

In addition to collections, Spino has a key value store. A value such as a numeric value or text can be stored in the database using a 'key'. The value can be retrieved later using the same key. 

```
db.setIntValue("portNumber", 1234);

var portNumber = db.getIntValue("portNumber");
```

