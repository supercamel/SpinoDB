#!/usr/bin/python3

import gi
import json

gi.require_version("Spino", "1.0")
from gi.repository import Spino

db = Spino.Database.new()
col = db.add_collection("test")

for i in range(10000000):
    doc = { "name": "Camel", "score": i }
    col.append(json.dumps(doc))
    print(i)

cursor = col.find("{name: \"Camel\"}").set_limit(1).set_projection(json.dumps({"name": 1}))


while(cursor.has_next()):
    print(cursor.next())
