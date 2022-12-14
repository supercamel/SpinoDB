#!/usr/bin/python3

import gi
import json
import time

gi.require_version("Spino", "1.2")
from gi.repository import Spino


db = Spino.Database.new()
col = db.get_collection("test")
col.create_index("i")

print("appending documents")

doc = { "name": "Camel", "species": "Human" }
col.append(json.dumps(doc))

doc = { "name": "Bruce", "species": "Shark" }
col.append(json.dumps(doc))

print("executing command")

db.execute(json.dumps({
    "cmd": "drop", 
    "collection": "test", 
    "query": "{$and:[{name:\"Bruce\"},{species:\"Shark\"}]}", 
    "limit": 1 
    }))

print("execute completed")

cursor = col.find("{}")

while(cursor.has_next()):
    print(cursor.next())

# add a million documents
for i in range(0, 1000000):
    doc = { "name": "Camel", "i": i }
    col.append(json.dumps(doc))

print("dropping")
# now drop them
start = time.time()
for i in range(0, 1):
    col.drop_one("{i:" + str(999999-i) + "}")
    print(i)
end = time.time()
print("dropped 1 million documents in " + str(end - start) + " seconds")