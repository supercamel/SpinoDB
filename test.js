const spino = require('./build/Release/spinodb.node');

var db = new spino.Spino();

var col = db.addCollection('test');

col.append({ idx: 0, number: 0 });

var entry = JSON.parse(col.findOne("{}"));
console.log(entry._id);
col.updateById(entry._id, JSON.stringify({ number: 1}));

console.log(col.findOne("{}"));
