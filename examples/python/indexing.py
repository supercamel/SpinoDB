#t!/usr/bin/python3

import gi
import json

import time

gi.require_version("Spino", "1.0")
from gi.repository import Spino

db = Spino.Database.new()
col = db.get_collection("test")
col.create_index("x")
col.create_index("y")

start = time.time()
for x in range(1000):
    for y in range(1000):
        doc = { "x": x, "y": y }
        col.append(json.dumps(doc))
end = time.time()
print(end - start)

start = time.time()
cursor = col.find("{$and: [{x: 100}, {y: {$gt: 500}}]}")
cursor.set_limit(1000)


while(cursor.has_next()):
    cursor.next()

end = time.time()
print(end - start)


