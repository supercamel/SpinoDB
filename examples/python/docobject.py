#!/usr/bin/python3

import gi
import json
import time

gi.require_version("Spino", "1.0")
from gi.repository import Spino

db = Spino.Database.new()
col = db.get_collection("people")
doc = Spino.DocObject.new()
arr = Spino.DocArray.new()

arr.add_int_element(1)
arr.add_string_element("camel")

print(arr.stringify())

subdoc = Spino.DocObject.new()
subdoc.add_string_member("id", "765611sadfs")

if(subdoc.has_member("id")):
    if(subdoc.get_member_type("id") == Spino.VALUETYPE.STRING):
        print(subdoc.get_string_member("id"))

start = time.time()
for i in range(100000):
    doc.clear()
    arr.add_int_element(1)
    arr.add_int_element(2)
    arr.add_int_element(3)


    doc.add_int_member("test", i)
    doc.add_string_member("name", "Sam")
    doc.add_array_member("count", arr)

    subdoc.add_string_member("id", "765611sadfs")
    doc.add_object_member("profile", subdoc)


    col.append_object(doc)

end = time.time()
print(end - start)

print(col.find_one("{test: 10}"))

db = Spino.Database.new()
col = db.get_collection("people")
start = time.time()
for i in range(100000):
    doc = { "test": i, "count":[1,2,3], "name": "Sam" }
    col.append(json.dumps(doc))

end = time.time()
print(end - start)




