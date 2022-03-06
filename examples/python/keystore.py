#!/usr/bin/python3

import gi
import json

gi.require_version("Spino", "1.0")
from gi.repository import Spino

db1 = Spino.Database.new()

db1.set_int_value("ikey", 1)
db1.set_uint_value("ukey", 2)
db1.set_double_value("doublekey", -12.5335)
db1.set_string_value("stringkey", "Hello world!!")

db1.save("dump.json")

db2 = Spino.Database.new()
db2.load("dump.json")

print(db2.get_int_value("ikey"))
print(db2.get_uint_value("ukey"))
print(db2.get_double_value("doublekey"))
print(db2.get_string_value("stringkey"))


