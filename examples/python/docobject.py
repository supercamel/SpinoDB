#!/usr/bin/python3

import gi
import json
import time

gi.require_version("Spino", "1.2")
from gi.repository import Spino

db = Spino.Database.new()
col = db.get_collection("people")

doc = col.create_node()
for i in range(100):
    doc.set_object()
    doc.add_int_member("i", i)
    col.append_node(doc)


val = col.find_one_view("{i:10}")
if(val.get_value_type() == Spino.VALUETYPE.OBJECT):
    print(val.stringify())






