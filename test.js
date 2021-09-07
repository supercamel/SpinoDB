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
var doc = col.findOne("{number: 500000}");
console.timeEnd("$eq");
console.log(doc);

doc = JSON.parse(doc);
console.time("index");
col.findOneById(doc._id);
console.timeEnd("index");

console.time("$ne");
col.findOne('{text: {$ne: "Test string"}}');
console.timeEnd("$ne");

console.time("$gt");
col.findOne('{subobject.num: {$gt: 9998}}');
console.timeEnd("$gt");

console.time("$lt");
console.log(col.findOne('{subobject.num: {$lt: 2}}'));
console.timeEnd("$lt");


