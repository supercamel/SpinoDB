#!/usr/bin/python3

import gi
import json

gi.require_version("Spino", "1.2")
from gi.repository import Spino

db1 = Spino.Database.new()

print("set int value")
db1.set_int_value("ikey", -15)
if(db1.get_int_value("ikey") != -15):
    print("error: set int value")
    exit(1)
print("set uint value")
db1.set_uint_value("ukey", 2)
print("set double key")
db1.set_double_value("doublekey", 12.5335)
print("set string key")
db1.set_string_value("stringkey", "Hello world!!")

print("getting double key")
print(db1.get_double_value("doublekey"))

print("saving")
db1.save("dump.json")
print("save done")

db2 = Spino.Database.new()

print("loading")
db2.load("dump.json")
print("load done")

print(db2.get_int_value("ikey"))
print(db2.get_uint_value("ukey"))
print(db2.get_double_value("doublekey"))
print(db2.get_string_value("stringkey"))


