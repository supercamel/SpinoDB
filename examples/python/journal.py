#!/usr/bin/python3

import gi
import json

gi.require_version("Spino", "1.0")
from gi.repository import Spino

for i in range(10):
    db = Spino.Database.new()
    db.load("db.db")
    db.enable_journal("journal")
    db.consolidate("db.db")

    col = db.get_collection("loopCountCollection")

    doc = { "loopNumber": i }

    col.append(json.dumps(doc))

    cur = col.find("{}")

    while(cur.has_next()):
        print(cur.next())

    print(col.find("{}").count())
