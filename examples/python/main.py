#!/usr/bin/python3

import gi
import json
import time

gi.require_version("Spino", "1.2")
from gi.repository import Spino

db = Spino.Database.new()
col = db.get_collection("test")
col.create_index("score")

# time how long it takes to add 1 million documents
start = time.time()
for i in range(1000000):
    doc = { "name": "Camel", "score": i }
    col.append(json.dumps(doc))
end = time.time()
print("Time to add 1 million documents: " + str(end - start))

start = time.time()
for i in range(1000000):
    #random number between 0 and 999999
    rand = int(time.time() * 1000000) % 1000000
    cursor = col.find_one("{score: " + str(rand) + "}")
stop = time.time()

print("Time to find 1 document: " + str(stop - start))

start = time.time()
db.save("test.db")
end = time.time()
print("Time to save database: " + str(end - start))

start = time.time()
db.load("test.db")
end = time.time()
print("Time to load database: " + str(end - start))