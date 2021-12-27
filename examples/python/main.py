#!/usr/bin/python3

import gi
import json

gi.require_version("Spino", "1.0")
from gi.repository import Spino

db = Spino.Database.new()
col = db.add_collection("test")

doc = { "name": "Camel", "score": 9001 }

col.append(json.dumps(doc))

cursor = col.find("{name: \"Camel\"}").set_limit(1).set_projection(json.dumps({"name": 1}))


while(cursor.has_next()):
    print(cursor.next())
