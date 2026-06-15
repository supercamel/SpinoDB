#!/usr/bin/env sqgi
/*
 * Small indexed lookup benchmark from SQGI.
 *
 * This is intentionally much smaller than the million-row Python/Vala demos,
 * so it can run quickly as an example.
 */

local GLib = import("GLib")
local Spino = import("Spino", "1.2")

function ms_since(start_us) {
    return (GLib.get_real_time() - start_us) / 1000
}

local db = Spino.Database.new()
local readings = db.get_collection("readings")
readings.create_index("device_id")
readings.create_index("site")

local n = (vargv.len() > 0) ? vargv[0].tointeger() : 25000
local start = GLib.get_real_time()

for (local i = 0; i < n; i++) {
    local device_id = "dev-" + (1000 + (i % 500))
    local site = (i % 2 == 0) ? "MEL" : "SYD"
    local payload = {
        device_id = device_id,
        site = site,
        reading = (i % 1000) / 10.0,
        captured_at = 1700000000 + i
    }
    readings.append(sqgi.json.stringify(payload))
}

print("inserted " + n + " readings in " + ms_since(start) + "ms\n")

local query = "{device_id:\"dev-1234\"}"
start = GLib.get_real_time()
for (local i = 0; i < 1000; i++) {
    readings.find_one(query)
}
print("1000 indexed find_one calls: " + ms_since(start) + "ms\n")

start = GLib.get_real_time()
local cursor = readings.find("{site:\"MEL\"}")
local matches = 0
while (cursor.has_next()) {
    local row = sqgi.json.parse(cursor.next())
    if (row.reading > 75) {
        matches++
    }
}
print("counted " + matches + " high MEL readings in " + ms_since(start) + "ms\n")
