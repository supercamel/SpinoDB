# SpinoDB

*work in progress*

*feels like its ready, lets call it 'beta' stage*

SpinoDB is an in-memory NoSQL database that is small and self-contained and with emphasis on **speed**. It is NodeJS addon written in C++ and is intended to be used with NodeJS for both web and desktop applications.


### When To Use It

##### Web Services

SpinoDB integrates nicely with Node & Express. By switching from an enterprise grade solution to SpinoDB, small/medium websites might notice improvements in website responsiveness and lower server CPU useage. 

Bots and online services that require fast document retrieval may find SpinoDB a more appropriate solution than a fully fledged enterprise system. 

SpinoDB may also be used as a fast cache to complement a traditional database.

##### Desktop Applications

Electron applications can use SpinoDB as an alternative to creating an ad-hoc format for application specific data.

##### Limitations

The practical limits are determined by 
- how much RAM your system has. It is an in memory database. 
- loading / saving time. If your system is reasonably spec'd you could budget ~100MB/s to load and save the database to a file.

As a rule of thumb, if the size of your data is greater than 50% of your available RAM, or if the save time is unmanageable, then this isn't for you. 

### Installation

    npm install spinodb
    

### Design

A SpinoDB contains 'collections'. A collection is roughly analogous to a table in an SQL database, except that instead of containing one dimensional rows, each entry in a collection contains a JSON document / Javascript object. The documents are not required to be consistent.


### Example

	const spino = require('spinodb');
    var db = new spino.Spino(); // create the db instance
    db.load('data.db'); // load from disk
    
    var userColl = db.getCollection("users"); // get the users collection
    if(userColl === undefined) { // if it does not exist, create it
        userColl = db.addCollection('users');
    }
    
    // retrieve a user called Dave
    var result = userColl.findOne('{ name: "Dave" });
    console.log(result); 

	// update Dave's score to 50. if the score field does not exist, it is created. 
    userColl.update('{ name: "Dave" }', '{"score": 50}'); 


### Collections

db.getCollection(<collection_name>) will return either the collection or undefined if it does not exist.

    var col = db.getCollection("users");

db.addCollection(<collection_name>) will add a collection to the database.

    var col = db.addCollection("users");

db.dropCollection(<collection_name>) will remove a collection from the database.

    db.dropCollection("users"); //destroy all user data


### Adding Documents

append() will add a JSON document to a collection.

    col.append({
        name: "Dave",
        score: 50,
        subdoc: {
	        item: "An item in a subdocument"
	        }
	     array: [ 1, 2, 3, 4]
        });

append() will accept either a Javascript object or JSON string.

### Find Queries

findOne() will retrieve exactly one document from the collection. The result is either a string of JSON data, or undefined if the query does not match any documents.

    result = collection.findOne(<query>);

find() will return a Cursor that can be used to iterate over the results.

    var cursor = collection.find(<query>);
    var doc = cursor.next();
    while(doc !== undefined) {
        // do something with the document
	    doc = cursor.next();
	 } 
    
Creating an array from a cursor can be done like this
  

    var json_array = '[';
    var val;
    while((val = cursor.next()) !== undefined) {
	    json_array += val;
	    json_array += ',';
	}
	array = array.slice(0, -1);
	array += ']';

The array is in string format ready to send over the web, or can be parsed into a javascript array with JSON.parse()

A cheeky alternative to using cursors is to use the command execution interface to make SpinoDB collate the results into an array for you.

	var array = db.execute({
		cmd: "find",
		collection: "collectionName",
		query: <query>
	});

    
    
### Updating Documents

update() will merge a JSON document into existing documents that match the search query. Existing fields will be overwritten and fields that do not exist will be created.

    collection.update(<query>, <updated_document>);

It isn't possible to delete a field with update(). To remove fields from a document, you must use find() to get a copy of the document, drop() to remove the document and then append() to insert a new document with the required format.

### Deleting Documents

dropOne() will delete exactly one document from the collection that matches the search query.

    collection.dropOne(<query>);

drop() will drop all documents that match the query.

    collection.drop(<query>);


dropOlderThan() will drop all documents older than a timestamp.

	collection.dropOlderThan(<time value>);

e.g. dropping all documents more than 2 weeks old

	let now = new Date().getTime();
	let twoWeeksAgo = now - (1000*60*60*24*14);	//2 weeks in milliseconds
	let nDocumentsDropped = collection.dropOlderThan(twoWeeksAgo);

### Query Format

SpinoDB has it's own query language. It is similar to the MongoDB query language, but it is not the same. 
Queries look like JSON strings or Javascript objects but they are not - they are strings in SpinoDB query language.

$eq - Equality operator will return documents if a field matches a value. 

    { name: {$eq: "Dave" }}
   
   If no operator is present then $eq is implied

    { name: "Dave" }

$ne - Not equal. Will return documents where the field does NOT match the value

    { name: {$ne: "Dave" }}
$gt - Greater than. Will only match documents if the field is a numeric value and is greater than the value

    {score: {$gt: 20}}
$lt - Less than. Will only match documents if the field is a numeric value and is less than the value

    {score: {$lt: 20}}

$in - Checks if a field contains one of many values. This example will return any document if the name field matches one of the specified names.

    {name: {$in: ["Dave", "Mike", "Alexis"]}}
$nin - Not in. Check if a field does not contain one of many values. This example will return any document that does not have one of the specified names. Results will exclude these names.

    {name: {$nin: ["Dave", "Mike", "Alexis"]}}
$exists - Checks if a field exists or not. If 'true' is passed as the parameter, then it will return documents that have the field. False will return documents that are missing the field.

    {name: {$exists: true}}
$type - Checks if a field is of a type. The type parameter can be either*number, string, bool, array, object*. This example will return all documents where the field 'name' is of type string.

    {name: {$type: string}}

### Logical Expressions

$and - can be used to check multiple attributes of the document. All sub expressions must be true for the document to match.

    {$and: [{name: "Dave"}, {score: {$gt: 20}}]}
This example gets every document with the name Dave and a score greater than 20.

$or - any sub expression can be true for the document to match.

    {$or: [{name: "Dave"}, {score: {$lt: 20}}]}

This example gets documents with the name Dave or if the score is less than 20.

$not - inverts the result of an expression

    {$not: {name: "Dave"}}
Matches documents where the name is not Dave.

### Sub Object Field Names

Field names are the basis of query operations and are used to identify data in a JSON document. JSON documents can contain objects inside of objects.

    {
        name: "Dave",
        steamProfile: {
                steamId: "7656112598325978325",
                steamPic: "https://steamcommunity.com/profile/picture/7656114829873"
            }
    }
To query the field of a sub object, the following syntax can be used

    { steamProfile.steamId: "7656112598325978325" }

### _id Field

When documents are first added to the database, they are given a unique ID. The ID encodes the document creation time which is used to optimize searching operations. For best performance, use the ByID functions.

	updateById(<id_string>, <newDocument>);

	findOneById(<id_string>);

	dropById(<id_string>);


### Indexing

Collections can be indexed. Indexing yields huge performance increases because the database can search for results far more efficiently. Note that indexed fields are not saved to disk and must be created in the application each time it runs.

col.createIndex(<field_name>);

	col.createIndex("steamProfile.steamId");


### Command Execution

A database administrator will, from time to time, have a requirement to run arbitrary queries on the data. SpinoDB does not follow the usual client-server model that would allow an administrator to simply login and run queries.

As an alternative, SpinoDB offers a command execution interface. This simplifies the creation of some kind of remote access because all the application needs to do is pass JSON objects or strings to the db.execute() function. A web server might have a secure URL end point that can be used to run queries with POSTman. 

Commands can be either JSON strings or Javascript objects. 

	var resultStr = db.execute(<string>);

	var resultObj = db.execute(<object>);


Examples:

	var result = db.execute({
		cmd: "createIndex",
		collection: "testCollection",
		field: "number"
	});

	var document = db.execute({
		cmd: "findById",
		collection: "testCollection",
		id: idString
	});

	var documentArray = db.execute({
		cmd: "find",
		collection: "testCollection",
		query: "{$and: [{number: {$gt: 10}}, {number: {$lt: 20}}]}"
	});

	var result = db.execute({
		cmd: "append",
		collection: "testCollection",
		document: JSON.stringify({
			number: 10,
			text: "a test object"
		})
	});

	var result = db.execute({
		cmd: "dropOne",
		collection: "testCollection",
		query: "{number: 10}"
	});

	var result = db.execute({
		cmd: "drop",
		collection: "testCollection",
		query: "{number: {$gt: 10}}"
	});

	var result = db.execute({
		cmd: "save",
		path: "data.db"
	});

The db.load() function does not have a corresponding command because loading should only occur once at start up. There should be no need to load from disk again even durnig long running execution.
	
### Performance Tuning

When SpinoDB executes a search, it goes through 3 stages
1. it will look up a cache of previously conducted queries and return a result from that. the cache is purged every time the collection is modified (appended, updated or a document dropped).
2. if the query is a basic comparison to an indexed field, it will conduct a binary search on the index. this operation is very fast, typically under 50us for a findOne()
3. finally, it will execute the query on every document. this is done linearly from the first document to the last. typically, might take a millisecond, but results vary. absolute worst case scenarious might take hundreds of milliseconds. 

* use the id field with the ById functions whenever possible. performing operations by id is by far the fastest.
* make sure you create indexes for fields you will be using to search for documents
* use findOne over find if you only expect to get 1 result. 

Prefer drop() over a series of calls to dropOne(). drop() can be used delete many documents but will only reconstruct the index once. 

If you have a high rate of data passing through a collection of millions of documents, drop time potentially could become a performance bottleneck. One strategy might be adding a field called pendingDelete to your documents that are ready for deletion. A search query to exclude 'deleted' documents might look like this.

	var cursor = col.find('{$and: [{name: "Dave"}, {pendingDelete: {$exists: false}}]}');

Then at some convenient moment, call 
	
    col.drop("{pendingDelete: {$exists: true}}");
    
Only bother with this is unless drop time becomes problematic.

### Comparison To LokiJS

LokiJS is a similar in-memory NoSQL database, however it is far more mature and featurefull. It also has the advantage of being a pure javascript solution and can run in browsers. 

##### Inserting 1 Million Documents (1x index field)

| LokiJS | SpinoDB |
|:-------|:--------|
| 1148ms |  2100ms |


LokiJS is about 2x faster at inserting new documents. This is most likely because SpinoDB requires objects to be stringified and reparsed . .. we working on this.

##### findOne by an indexed field

| LokiJS | SpinoDB |
|--------|---------|
| 0.459ms|0.049ms  |


##### drop/remove by an indexed field

| LokiJS  | SpinoDB |
|---------|---------|
| 2075ms  | 404ms   |

Drop/remove is expensive, probably because the index has to be reconstructed

### Persistence

The database can be loaded and stored to a file.
db.load(<path_to_file>);

    db.load("data.db");
db.save(<path_to_file>);

    db.save("data.db");
Note that these functions are synchronous / blocking functions. Once your database exceeds 100+MB you may start to notice lags during saves. 

For reference, SpinoDB will parse/stringify about 100MB per second from file on my PC with a mediocre SATA SSD, . 

