#!/usr/bin/python3

import gi
import json
import random

gi.require_version("Spino", "1.0")
from gi.repository import Spino

db = Spino.Database.new()
col = db.add_collection("trains")

for i in range(20):
    year = random.randint(1930, 2020)
    doc = { "name": "TrainyMcTrainface", "year": year }
    col.append(json.dumps(doc))

projection = { "year": 1 }
result = col.find("{}").set_projection(json.dumps(projection)).run_script("""
trains <- [];
function result(r) {
    if(trains.len() < 5) {
        trains.append(r);
        trains.sort(@(a,b) a.year <=> b.year);
    }
    else {
        if(r.year < trains[4].year) {
            trains[4] = r;
            trains.sort(@(a,b) a.year <=> b.year);
	}
    }
}

function finalize() {
    return trains;
}
""")

print(result)
