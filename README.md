# SpinoDB

SpinoDB is an in-memory NoSQL database that is small and self-contained and with emphasis on **speed**. It is NodeJS addon written in C++ and is intended to be used with NodeJS for both web applications and desktop applications that require either the performance of an in-memory database or the ease of distribution that comes with using a self-contained database. It may also be used as a fast cache to complement a traditional database.

SpinoDB is designed for speed. It can parse a database file at ~100MB/s. A simple findOne() query on a collection of 10 million documents will typically return a result within 200m/s. Small collections will return results in microseconds.

The maximum amount of data that SpinoDB can store depends on your systems available RAM. Generally SpinoDB is suitable for managing up to a few GB of data.

**Design**

A SpinoDB contains 'collections'. A collection is roughly analogous to a table in an SQL database, except that instead of containing one dimensional rows, each entry in a collection contains a JSON document. The documents are not required to be consistent.

Documents are JSON strings. To convert to/from Javascript objects you must use JSON.stringify() and JSON.parse().

**Installation**

    npm install spinodb

**Example**

	const spino = require('spinodb');
    var db = new spino.Spino(); // create the db instance
    db.load('data.db'); // load from disk
    
    var userColl = db.getCollection("users"); // get the users collection
    if(userColl === undefined) { // if it does not exist, create it
        userColl = db.addCollection('users');
    }
    
    var result = userColl.findOne('{ name: "Dave" }); // retrieve a user called Dave
    console.log(result); 

    userColl.update('{ name: "Dave" }', '{"score": 50}'); // update Dave's score to 50. if the score field does not exist, it is created. 


**Collections**

db.getCollection(<collection_name>) will return either the collection or undefined if it does not exist.

    var col = db.getCollection("users");
db.addCollection(<collection_name>) will add a collection to the database.

    var col = db.addCollection("users");
db.dropCollection(<collection_name>) will remove a collection from the database.

    db.dropCollection("users"); //destroy all user data

**Adding Documents**

append() will add a JSON document to a collection.

    col.append(JSON.stringify({
        name: "Dave",
        score: 50,
        subdoc: {
	        item: "An item in a subdocument"
	        }
	     array: [ 1, 2, 3, 4]
        }));

append() will also accept a Javascript object, however it is about 15% faster to call JSON.stringify() on the object to convert it to a string. Note that append is quite slow because it requires memory allocation and the JSON object must be reparsed into the internal DOM structure. This is a target for future optimisation, however at present SpinoDB can append approximately 150k simple documents per second. 

**Find Queries**

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
    
    
**Updating Documents**

update() will merge a JSON document into existing documents that match the search query. Existing fields will be overwritten and fields that do not exist will be created.

    collection.update(<query>, <updated_document>);

It isn't possible to delete a field with update(). To remove fields from a document, you must use find() to get a copy of the document, drop() to remove the document and then append() to insert a new document with the required format.

**Deleting Documents**

dropOne() will delete exactly one document from the collection that matches the search query.

    collection.dropOne(<query>);

drop() will drop all documents that match the query.

    collection.drop(<query>);

**Query Format**

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

**Logical Expressions**

$and - can be used to check multiple attributes of the document. All sub expressions must be true for the document to match.

    {$and: [{name: "Dave"}, {score: {$gt: 20}}]}
This example gets every document with the name Dave and a score greater than 20.

$or - any sub expression can be true for the document to match.

    {$or: [{name: "Dave"}, {score: {$lt: 20}}]}

This example gets documents with the name Dave or if the score is less than 20.

$not - inverts the result of an expression

    {$not: {name: "Dave"}}
Matches documents where the name is not Dave.

**Sub Object Field Names**

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

**id Field**

When documents are first added to the database, they are given a unique ID. The ID encodes the document creation time which can be used to optimize findOne() operations. For best performance, search for documents by the 'id' and do not overwrite it. 

**Persistence**

The database can be loaded and stored to a file.
db.load(<path_to_file>);

    db.load("data.db");
db.save(<path_to_file>);

    db.save("data.db");
Note that these functions are synchronous / blocking functions. Once your database exceeds 100+MB you may start to notice lags during saves. Use them judiciously. 

For reference, SpinoDB will parse/stringify about 100MB per second from file on my PC with a mediocre SATA SSD, . 

