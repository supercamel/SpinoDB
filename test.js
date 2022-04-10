/**
 *
 * This is a performance comparison between NodeJS and Spino's
 * Squirrel-powered cursor scripts.
 *
 * In this test, we populate a collection with 100k entries of 'solar systems'. 
 * Each solar system has some planets.
 * We have to produce an ordered list of the 5 closest possibly inhabital planets. 
 *
 * Both the Javascript and Squirrel versions use a find("{}") query.
 * The Javascript version iterates over each document produces by the cursor.
 * Both scripts do essentially the same thing. 
 *
 * On my PC the results are
 *  nativejs: 422ms
 *  squirrel: 273ms
 *
 * It seems cursor scripts are about 30% faster, in this case. 
 *
 * Since we are looking for inhabitable planets around the closest starts, 
 * some further improve performance consider are
 * 1. Indexing the 'dist' field
 * 2. Query for dist less than some arbitary amount to trim the amount of query results
 * This can improve query speed 50x which is far more significant than script vs native.
 *
 */


const spino = require('./build/Release/spinodb.node');


var db = new spino.Spino();

let c = db.getCollection("solarsystems");
//c.createIndex("dist");

function getRandomInt(max) {
    return Math.floor(Math.random() * max);
}


function generateRandomSolarSystems(col) {
    var starTypes = ["solar", "hotblue", "reddwarf", "redgiant", "whitedwarf", "neutron"];
    var planetTypes = ["rock", "gas"];

    for(let i = 0; i < 100000; i++) {
        let planets = [];
        let n_planets = getRandomInt(6);

        for(let j = 0; j < n_planets; j++) {
            let planet = {
                type: planetTypes[Math.floor(Math.random()*2)],
                dist: Math.random()*10.0
            };
            planets.push(planet);
        }

        let solarSystem = {
            name: getRandomInt(1000000).toString(),
            type: starTypes[getRandomInt(5)],
            dist: 100+getRandomInt(1000000),
            planets: planets
        }

        col.append(JSON.stringify(solarSystem));
    }
}

function doTheThing(col) {
    //var cursor = col.find("{dist:{$lt:2000}}");
    var cursor = col.find("");
    var goldilocksZoned = [];
    while(cursor.hasNext()) {
        var ss = JSON.parse(cursor.next());
        for(var n in ss.planets) {
            var planet = ss.planets[n];
            if(planet.type == "rock") {
                if((planet.dist > 3.2) && (planet.dist < 3.8)) {
                    let p = {
                        starName: ss.name,
                        planet: n,
                        dist: ss.dist
                    };
                    goldilocksZoned.push(p);
                }

            }
        }
    }

    goldilocksZoned.sort((a, b) => {  
        if(a.dist > b.dist) {
            return 1;
        }
        else {
            return -1;
        }
    });

    goldilocksZoned = goldilocksZoned.slice(0, 5);
    console.log(goldilocksZoned);
}

function doTheScript(col) {
    var script = `
goldilocksZoned <- [];
function result(ss) {
    local count = 0;
    foreach(p in ss.planets) {
        if(p.type == "rock") {
            if((p.dist > 3.2) && 
                (p.dist < 3.8)) {
                local pl = {
                    "star": ss.name,
                    "planet": count,
                    "dist": ss.dist
                }
                goldilocksZoned.append(pl);
            }
        }
        count++;
    }
}

function finalize() {
    goldilocksZoned.sort(@(a, b) a.dist <=> b.dist);
    if(goldilocksZoned.len() > 5) {
        goldilocksZoned.resize(5);
    }
    return goldilocksZoned;
}
`;

    var cursor = col.find("{}");
    console.log(cursor.runScript(script));
}



generateRandomSolarSystems(c);


console.time('nativejs')
doTheThing(c);
console.timeEnd('nativejs')

console.time('squirrel');
doTheScript(c);
console.timeEnd('squirrel');

