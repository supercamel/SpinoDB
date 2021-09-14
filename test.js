const spino = require('./build/Release/spinodb.node');

var db = new spino.Spino();

//db.load("test.db");

var col = db.addCollection('test');


col.append({
    test: "Hello world"
});


console.log(col.findOne('{test: {$startsWith: "Hello"}}'));

/*
console.log(db.execute({
	cmd: "find",
	collection: "test",
    query: '{test: {$startsWith: "Hello"}}'
}));


/*
col.append(JSON.stringify({
	test: 1
}));

var one = JSON.parse(col.findOne("{test: 1}"));

console.log(col.timestampById(one._id));


console.log(db.execute({
	cmd: "count"
}));



console.time("Adding documents");
for(var i = 0; i < 1000000; i++) {
	db.execute({
		cmd: "append",
		collection: "test",
		document: JSON.stringify({
			number: i,
			text: "Test string",
			subobject: {
				field: "A field",
				bool: true,
				num: i
			}
		})
	});
}

console.log(db.execute({
	cmd: "count",
	collection: "test"
}));

console.log(db.execute({
	cmd: "find",
	collection: "test",
	query: "{$and: [{number: {$gt:10}}, {number: {$lt:15}}]}"
}));

console.log(col.findOne("{number: 10}"));

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
console.timeEnd("drop unindexed");

console.log(col.findOne("{number: 5}"));

console.time("$and");
col.findOne("{$and: [{number: 900000}, {subobject.bool: true}]}");
console.timeEnd("$and");


/*
 * */

