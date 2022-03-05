const spino = require('./build/Release/spinodb.node');


var db = new spino.Spino();

var col = db.addCollection('test');
//col.createIndex("idx");

for(var i = 0; i < 10; i++) {
    col.append({ idx: i.toString(), number: 0, subobj: { p: 1, q: 2} });
}

var input = "\"}";
col.drop('{ idx: "' + input + '"}');


var cursor = col.find("{}");
while(cursor.hasNext()) {
    console.log(cursor.next());
}
