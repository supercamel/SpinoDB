# Query Language

SpinoDB has a query language for matching documents. Spino queries are used exclusively for matching documents in find, update and drop operations. Projection, aggregation, sorting and formatting is not done using the query language but can be done either in the application or through cursor scripts. 

Spino queries have a similar format and appearance to JSON, though this similarity is superficial. The left hand side is not enclosed in quotes and it isn't properly formatted JSON. You must build a string to create the query.

## Operators

Empty brackets will match all documents in a collection. 

    { }

### Equal
 
$eq - Equality operator will return documents if a field matches a value. 

    { name: {$eq: "Dave" }}
   
   If no operator is present then $eq is implied

    { name: "Dave" }

### Not equal 

$ne - Not equal. Will return documents where the field does NOT match the value

    { name: {$ne: "Dave" }}

### Greater than

$gt - Greater than. Will only match documents if the field is a numeric value and is greater than the value

    {score: {$gt: 20}}

### Less than

$lt - Less than. Will only match documents if the field is a numeric value and is less than the value

    {score: {$lt: 20}}

### Range

$range - Check if a value is within a range

    {score: {$range:[0, 20]}}


### In

$in - Checks if a field contains one of many values. This example will return any document if the name field matches one of the specified names.

    {name: {$in: ["Dave", "Mike", "Alexis"]}}

### Not In

$nin - Not in. Check if a field does not contain one of many values. This example will return any document that does not have one of the specified names. Results will exclude these names.

    {name: {$nin: ["Dave", "Mike", "Alexis"]}}

### Exists

$exists - Checks if a field exists or not. If 'true' is passed as the parameter, then it will return documents that have the field. False will return documents that are missing the field.

    {name: {$exists: true}}
$type - Checks if a field is of a type. The type parameter can be either*number, string, bool, array, object*. This example will return all documents where the field 'name' is of type string.

    {name: {$type: string}}

### Starts With
$startsWith - will get documents with a string field that starts with the characters specified. Case sensitive.

    {name: {$startsWith: "D"}}

### Regex

$regex - will match a string field against a regex query. The regex flavour is ECMAScript, not PCRE. ECMAScript does not allow for case insensitive searching, so SpinoDB has borrowed the "(?i)" modifier to do this. Any regex query that begins with (?i) will make the search case insensitive. 

    {name: {$regex: "^D.*"}}

    {name: {$regex: "(?i)d"}}

 
## Logical Expressions

### And

$and - can be used to check multiple attributes of the document. All sub expressions must be true for the document to match.

    {$and: [{name: "Dave"}, {score: {$gt: 20}}]}
This example gets every document with the name Dave and a score greater than 20.

### Or

$or - any sub expression can be true for the document to match.

    {$or: [{name: "Dave"}, {score: {$lt: 20}}]}

This example gets documents with the name Dave or if the score is less than 20.

### Not

$not - inverts the result of an expression

    {$not: {name: "Dave"}}
Matches documents where the name is not Dave.

## Sub Object Field Names

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

