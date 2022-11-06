#!/usr/bin/python3

import gi
import json

gi.require_version("Spino", "1.2")
from gi.repository import Spino

db = Spino.Database.new()
db.load("data.db")
db.enable_journal("journal")
db.consolidate("data.db")

print(db.get_string_value("test_key"))
db.set_string_value("test_key", "test_value")



