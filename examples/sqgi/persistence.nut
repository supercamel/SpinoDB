#!/usr/bin/env sqgi
/*
 * Save/load, key-value metadata, and journal recovery.
 *
 * The journal section simulates a process writing changes after the last full
 * save, then a later process loading the base DB and consolidating the journal.
 */

local GLib = import("GLib")
local Spino = import("Spino", "1.2")
local system = import("system")

function json(value) {
    return sqgi.json.stringify(value)
}

local stamp = GLib.get_real_time()
local db_path = system.paths.tmp + "/spinodb_sqgi_persistence_" + stamp + ".db"
local journal_path = system.paths.tmp + "/spinodb_sqgi_persistence_" + stamp + ".journal"

local seed = Spino.Database.new()
seed.set_string_value("pipeline", "daily-fulfilment")
seed.set_int_value("batch", 41)

local jobs = seed.get_collection("jobs")
jobs.create_index("id")
jobs.create_index("state")
jobs.append(json({ id = "job-1001", state = "done", queue = "shipments" }))
seed.save(db_path)

print("saved base database: " + db_path + "\n")

local writer = Spino.Database.new()
writer.load(db_path)
writer.enable_journal(journal_path)

local queued = writer.get_collection("jobs")
queued.append(json({ id = "job-1002", state = "queued", queue = "shipments" }))
queued.append(json({ id = "job-1003", state = "queued", queue = "invoices" }))
queued.update("{id:\"job-1002\"}", json({ state = "running", worker = "fulfilment-2" }))

print("wrote live changes to journal: " + journal_path + "\n")

local recovered = Spino.Database.new()
recovered.load(db_path)
recovered.enable_journal(journal_path)
recovered.consolidate(db_path)

local recovered_jobs = recovered.get_collection("jobs")
print("after journal consolidate: " + recovered_jobs.get_size() + " jobs\n")
print("pipeline: " + recovered.get_string_value("pipeline")
      + ", batch " + recovered.get_int_value("batch") + "\n")

local cursor = recovered_jobs.find("{state:{$ne:\"done\"}}")
while (cursor.has_next()) {
    local job = sqgi.json.parse(cursor.next())
    print("  pending " + job.id + " in " + job.queue + " -> " + job.state + "\n")
}
