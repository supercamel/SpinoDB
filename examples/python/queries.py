#!/usr/bin/python3

import gi
import json

gi.require_version("Spino", "1.0")
from gi.repository import Spino

db = Spino.Database.new()
col = db.add_collection("test")

doc = { "name": "Camel", "score": 9001, "secret_field": 1 }
col.append(json.dumps(doc))

doc = { "name": "Steve", "score": 19001 }
col.append(json.dumps(doc))


def greaterAndLessThan():
    cursor = col.find("{score: {$gt: 12000}}").\
        set_limit(1).\
        set_projection(json.dumps({"name": 1}))

    n = json.loads(cursor.next())["name"]
    if(n != "Steve"):
        print("$gt test failed")
    cursor = col.find("{score: {$lt: 12000}}").\
        set_limit(1).\
        set_projection(json.dumps({"name": 1}))
    n = json.loads(cursor.next())["name"]
    if(n != "Camel"):
        print("$lt test failed")

def eqAndNeq():
    result = json.loads(col.find_one("{name: {$eq: \"Camel\"}}"))
    if(result["name"] != "Camel"):
        print("$eq test failed")

    result = json.loads(col.find_one("{name: {$ne: \"Camel\"}}"))
    if(result["name"] != "Steve"):
        print("$neq test failed")

def inNin():
    n = col.find("{name: {$in: [\"Horse\", \"Camel\", \"Donkey\"]}}").count()
    """
    if(n != 1):
        print("$in test failed")
    n = col.find_one("{name: {$nin: [\"Horse\", \"Camel\", \"Donkey\"]}}")

    if(json.loads(n)["name"] != "Steve"):
        print("$nin test failed")
    """

def existsTypeStartsWith():
    n = col.find("{secret_field: {$exists: true}}")
    if(json.loads(n.next())["name"] != "Camel"):
        print("$exists test failed")
    n = col.find("{super_secret_field: {$exists: true}}")
    if(n.count() != 0):
        print("$exists test failed")

    if(col.find("{name: {$type: string}}").count() != 2):
        print("$type test failed")
    if(col.find("{name: {$type: number}}").count() != 0):
        print("$type test failed")

    if(col.find("{name: {$startsWith: \"C\"}}").count() != 1):
        print("$startsWith test failed")

    

greaterAndLessThan()
eqAndNeq()
inNin()
existsTypeStartsWith()



