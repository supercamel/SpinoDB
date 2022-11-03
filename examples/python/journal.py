#!/usr/bin/python3

import gi
import json

gi.require_version("Spino", "1.2")
from gi.repository import Spino

for i in range(3):
    db = Spino.Database.new()
    db.load("db.db")
    db.enable_journal("journal")
    db.consolidate("db.db")

    col = db.get_collection("loopCountCollection")

    doc = { "loop\nNumber": i }

    col.append(json.dumps(doc))

    cur = col.find("{}")

    while(cur.has_next()):
        print(cur.next())

    print(col.find("{}").count())

    print("FINISHED")
