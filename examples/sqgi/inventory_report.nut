#!/usr/bin/env sqgi
/*
 * A small inventory report using SpinoDB as the in-memory document store and
 * SQGI as the reporting language.
 */

local Spino = import("Spino", "1.2")

function json(value) {
    return sqgi.json.stringify(value)
}

local db = Spino.Database.new()
local stock = db.get_collection("stock")
stock.create_index("sku")
stock.create_index("warehouse")
stock.create_index("active")

local rows = [
    { sku = "bat-12v-7ah", warehouse = "MEL", category = "battery", on_hand = 38, reserved = 21, reorder_point = 20, unit_cost = 18.70, supplier = "Voltcraft", active = true },
    { sku = "cable-usbc-1m", warehouse = "MEL", category = "cable", on_hand = 240, reserved = 60, reorder_point = 80, unit_cost = 2.10, supplier = "Brightwire", active = true },
    { sku = "sensor-temp-pro", warehouse = "SYD", category = "sensor", on_hand = 14, reserved = 9, reorder_point = 12, unit_cost = 42.00, supplier = "Northwind Electrical", active = true },
    { sku = "sensor-door-mini", warehouse = "MEL", category = "sensor", on_hand = 5, reserved = 2, reorder_point = 8, unit_cost = 24.50, supplier = "Northwind Electrical", active = true },
    { sku = "relay-240v", warehouse = "BNE", category = "control", on_hand = 96, reserved = 10, reorder_point = 25, unit_cost = 9.75, supplier = "SwitchLab", active = true },
    { sku = "modem-lte-industrial", warehouse = "MEL", category = "network", on_hand = 7, reserved = 6, reorder_point = 10, unit_cost = 118.40, supplier = "FieldLink", active = true },
    { sku = "label-old-roll", warehouse = "MEL", category = "label", on_hand = 3, reserved = 0, reorder_point = 0, unit_cost = 1.20, supplier = "Archive", active = false }
]

foreach (row in rows) {
    stock.append(json(row))
}

local report = []
local cursor = stock.find("{active:true}")
while (cursor.has_next()) {
    local item = sqgi.json.parse(cursor.next())
    local available = item.on_hand - item.reserved
    if (available <= item.reorder_point) {
        report.append({
            sku = item.sku,
            warehouse = item.warehouse,
            category = item.category,
            supplier = item.supplier,
            available = available,
            reorder_point = item.reorder_point,
            short_by = item.reorder_point - available,
            restock_value = (item.reorder_point - available) * item.unit_cost
        })
    }
}

report.sort(function(a, b) {
    if (a.short_by == b.short_by) return a.sku <=> b.sku
    return b.short_by <=> a.short_by
})

print("restock watchlist:\n")
foreach (item in report) {
    print("  " + item.sku + " @ " + item.warehouse
          + " available=" + item.available
          + " reorder=" + item.reorder_point
          + " short_by=" + item.short_by
          + " value=$" + format("%.2f", item.restock_value)
          + "\n")
}

print("\njson report:\n")
print(sqgi.json.stringify(report, 2) + "\n")
