#!/usr/bin/python3

import gi
import json
import time

gi.require_version("Spino", "1.2")
from gi.repository import Spino

db = Spino.Database.new()
col = db.get_collection("people")

doc = Spino.DocNode.new()
for i in range(100):
    doc.set_object()
    doc.add_int_member("i", i)
    col.append_node(doc)


val = col.find_one_view("{\"i\":10}")
if(val.get_value_type() == Spino.VALUETYPE.OBJECT):
    print(val.stringify())



arr = Spino.DocNode.new()
arr.set_array()
for i in range(100):
    val = Spino.DocNode.new()
    val.set_int(i)
    arr.append(val)

iter = arr.get_view().begin()
while(iter.is_not(arr.get_view().end())):
    val = iter.get_view()
    iter.next()


dictionary = {
    "name": "John",
    "age": 30,
    "city": "New York"
}
with open("sample.json", "w") as outfile:
    json.dump(dictionary, outfile)

startTime = time.time()
for i in range(10000):
    doc = Spino.DocNode.new_from_json_file("sample.json")
endTime = time.time()
print("Time to parse 10000 JSON files: " + str(endTime - startTime) + " seconds")

startTime = time.time()
for i in range(10000):
    # load a JSON file using the json module
    with open("sample.json", "r") as infile:
        dictionary = json.load(infile)

endTime = time.time()
print("Time to parse 10000 JSON files using the json module: " + str(endTime - startTime) + " seconds")
