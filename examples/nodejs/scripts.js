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
 *  nativejs: 408ms
 *  squirrel: 258ms
 *
 * It seems cursor scripts are significantly faster, in this case. 
 *
 */


const spino = require('./build/Release/spinodb.node');


var db = new spino.Spino();

let c = db.getCollection("solarsystems");

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
    var cursor = col.find("{}");
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



console.time('squirrel');
generateRandomSolarSystems(c);


console.time('nativejs')
doTheThing(c);
console.timeEnd('nativejs')


doTheScript(c);
console.timeEnd('squirrel');

