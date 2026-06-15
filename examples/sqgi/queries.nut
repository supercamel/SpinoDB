#!/usr/bin/env sqgi
/*
 * Query operator tour from SQGI.
 *
 * The dataset is deliberately tiny, so each query can double as an executable
 * example and a smoke test.
 */

local Spino = import("Spino", "1.2")

function json(value) {
    return sqgi.json.stringify(value)
}

function check_count(collection, label, query, expected) {
    local actual = collection.find(query).count()
    if (actual != expected) {
        throw label + " expected " + expected + ", got " + actual
    }
    print(format("%-18s %2d  %s\n", label, actual, query))
}

local db = Spino.Database.new()
local incidents = db.get_collection("incidents")
incidents.create_index("id")
incidents.create_index("service")
incidents.create_index("severity")
incidents.create_index("status")

local rows = [
    {
        id = "inc-1001",
        service = "checkout",
        customer = "ACME Retail",
        status = "open",
        severity = 2,
        owner = "payments",
        page_on_call = true,
        error_code = "PAY-42",
        tags = ["payments", "mobile"]
    },
    {
        id = "inc-1002",
        service = "worker",
        customer = "Harbour Logistics",
        status = "investigating",
        severity = 4,
        owner = "platform",
        page_on_call = true,
        error_code = "JOB-7",
        tags = ["queue", "backlog"]
    },
    {
        id = "inc-1003",
        service = "search",
        customer = "City Library",
        status = "resolved",
        severity = 1,
        owner = "search",
        page_on_call = false,
        postmortem_url = "https://status.example/inc-1003",
        tags = ["indexing"]
    },
    {
        id = "inc-1004",
        service = "crm",
        customer = "Internal Tools",
        status = "open",
        severity = 3,
        page_on_call = false,
        tags = ["admin", "permissions"]
    }
]

foreach (row in rows) {
    incidents.append(json(row))
}

print("query checks:\n")
check_count(incidents, "$gt", "{severity:{$gt:2}}", 2)
check_count(incidents, "$lt", "{severity:{$lt:3}}", 2)
check_count(incidents, "$eq", "{status:{$eq:\"open\"}}", 2)
check_count(incidents, "$ne", "{status:{$ne:\"resolved\"}}", 3)
check_count(incidents, "$in", "{service:{$in:[\"checkout\",\"search\"]}}", 2)
check_count(incidents, "$nin", "{service:{$nin:[\"checkout\",\"search\"]}}", 2)
check_count(incidents, "$exists true", "{postmortem_url:{$exists:true}}", 1)
check_count(incidents, "$exists false", "{owner:{$exists:false}}", 1)
check_count(incidents, "$type", "{tags:{$type:array}}", 4)
check_count(incidents, "$startsWith", "{customer:{$startsWith:\"ACME\"}}", 1)

print("\ncurrently paging someone:\n")
local active = incidents.find("{page_on_call:true}")
while (active.has_next()) {
    local row = sqgi.json.parse(active.next())
    print("  " + row.id + "  sev" + row.severity + "  " + row.service + "\n")
}
