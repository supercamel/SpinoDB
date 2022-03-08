#!/usr/bin/python3

import gi
import json

gi.require_version("Spino", "1.0")
from gi.repository import Spino

db = Spino.Database.new()
col = db.add_collection("test")

doc = { "name": "Camel", "score": None}

col.append(json.dumps(doc))

doc["name"] = "Horse"
col.append(json.dumps(doc))

cursor = col.find("{}")

result = cursor.run_script("""
names <- [];
function result(r) {
    names.append(r.name);
}

function finalize() {
    return names;
}
""")

print(result)
