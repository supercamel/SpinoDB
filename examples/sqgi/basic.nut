#!/usr/bin/env sqgi
/*
 * Basic SpinoDB usage from SQGI.
 *
 * This keeps the data small enough to read, but models the usual flow:
 * create indexes, append JSON documents, query, update, drop, and save/load.
 */

local Spino = import("Spino", "1.2")
local system = import("system")

function json(value) {
    return sqgi.json.stringify(value)
}

function order_query(order_id) {
    return "{order_id:\"" + Spino.escape(order_id) + "\"}"
}

local db = Spino.Database.new()
local orders = db.get_collection("orders")
orders.create_index("order_id")
orders.create_index("customer")
orders.create_index("status")

local seed_orders = [
    {
        order_id = "ord-1001",
        customer = "Northwind Electrical",
        status = "new",
        priority = 2,
        city = "Melbourne",
        total = 1487.50,
        due_days = 3,
        tags = ["field", "metering"]
    },
    {
        order_id = "ord-1002",
        customer = "Bayside Health",
        status = "packed",
        priority = 1,
        city = "Geelong",
        total = 342.15,
        due_days = 5,
        tags = ["clinic", "consumables"]
    },
    {
        order_id = "ord-1003",
        customer = "ACME Cold Storage",
        status = "new",
        priority = 3,
        city = "Melbourne",
        total = 2310.00,
        due_days = 1,
        tags = ["urgent", "refrigeration"]
    },
    {
        order_id = "ord-1004",
        customer = "Laneway Grocers",
        status = "cancelled",
        priority = 0,
        city = "Ballarat",
        total = 91.40,
        due_days = 10,
        tags = ["retail"]
    }
]

foreach (order in seed_orders) {
    orders.append(json(order))
}

print("orders in memory: " + orders.get_size() + "\n")

print("\nopen high-value orders:\n")
local cursor = orders.find("{status:{$ne:\"cancelled\"}}")
while (cursor.has_next()) {
    local order = sqgi.json.parse(cursor.next())
    if (order.total > 1000) {
        print("  " + order.order_id + "  " + order.customer
              + "  $" + format("%.2f", order.total) + "\n")
    }
}

orders.update(order_query("ord-1003"), json({
    status = "in_progress",
    assigned_to = "ari",
    updated_at = "2026-06-15T09:30:00+10:00"
}))

local changed = sqgi.json.parse(orders.find_one(order_query("ord-1003")))
print("\nupdated " + changed.order_id + ": "
      + changed.status + " / " + changed.assigned_to + "\n")

local removed = orders.drop("{status:\"cancelled\"}")
print("removed cancelled orders: " + removed + "\n")

local db_path = system.paths.tmp + "/spinodb_sqgi_orders.db"
db.save(db_path)

local loaded_db = Spino.Database.new()
loaded_db.load(db_path)
local loaded_orders = loaded_db.get_collection("orders")
print("loaded from " + db_path + ": " + loaded_orders.get_size() + " orders\n")
