#!/usr/bin/python3

import gi
import json

import time

gi.require_version("Spino", "1.0")
from gi.repository import Spino

db = Spino.Database.new()
col = db.add_collection("test")
col.create_index("x")
col.create_index("y")

for x in range(1000):
    for y in range(1000):
        doc = { "x": x, "y": y }
        col.append(json.dumps(doc))

start = time.time()
cursor = col.find("{$and: [{x: 100}, {y: 500}]}").set_limit(1)
end = time.time()
print(end - start)




while(cursor.has_next()):
    print(cursor.next())
