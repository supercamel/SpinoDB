const spino = require('./build/Release/spinodb.node');

var db = new spino.Spino();

var col = db.addCollection('test');
//col.createIndex("idx");

for(var i = 0; i < 100; i++) {
    col.append({ idx: i, number: 0, subobj: { p: 1, q: 2} });
}

var entry = col.find("{idx: 0}").setProjection(JSON.stringify({idx:  1})).setLimit(5);
while(entry.hasNext()) {
    console.log(entry.next());
}

