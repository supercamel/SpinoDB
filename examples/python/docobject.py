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
    print(val.get_int())
    iter.next()


