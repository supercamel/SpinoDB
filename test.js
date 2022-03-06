const spino = require('./build/Release/spinodb.node');


var db = new spino.Spino();

db.setIntValue("i", 10);
console.log(db.getIntValue("i"));

db.save("db.json");
