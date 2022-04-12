#!/usr/bin/python3

import gi
import json

gi.require_version("Spino", "1.0")
from gi.repository import Spino

db = Spino.Database.new()
col = db.add_collection("test")
col.create_index("name")
col.create_index("species")

doc = { "name": "Camel", "species": "Human" }
col.append(json.dumps(doc))

doc = { "name": "Bruce", "species": "Shark" }
col.append(json.dumps(doc))

db.execute(json.dumps({
    "cmd": "drop", 
    "collection": "test", 
    "query": "{$and:[{name:\"Bruce\"},{species:\"Shark\"}]}", 
    "limit": 4294967295 
    }))

cursor = col.find("{}")

while(cursor.has_next()):
    print(cursor.next())
