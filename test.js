const spino = require('./build/Release/spinodb.node');

var db = new spino.Spino();

//db.load("test.db");

var col = db.addCollection('test');

col.createIndex("number");

console.time("Adding documents");
for(var i = 0; i < 1000000; i++) {
	col.append(JSON.stringify({
		number: i,
		text: "Test string",
		subobject: {
			field: "A field",
			bool: true,
			num: i
		}
	}));
}

//db.save("test.db");
console.timeEnd("Adding documents");

console.time("$eq");
var doc_str = col.findOne("{number: 999999}");
console.timeEnd("$eq");

var doc = JSON.parse(doc_str);
console.time("findOneById");
col.findOneById(doc._id);
console.timeEnd("findOneById");

console.time("$ne");
col.findOne('{text: {$ne: "Test string"}}');
console.timeEnd("$ne");

console.time("$gt");
col.find('{subobject.num: {$gt: 9998}}');
console.timeEnd("$gt");

console.time("$lt");
console.log(col.findOne('{subobject.num: {$lt: 2}}'));
console.timeEnd("$lt");

var doc = JSON.parse(col.findOne("{number: 5}"));

console.log(doc._id);

console.time("drop by id");
col.dropById(doc._id);
console.timeEnd("drop by id");

console.time("drop unindexed");
col.drop("{subobject.num: 1000}");
console.time("drop unindexed");

console.log(col.findOne("{number: 5}"));

console.time("$and");
col.findOne("{$and: [{number: 900000}, {subobject.bool: true}]}");
console.timeEnd("$and");
