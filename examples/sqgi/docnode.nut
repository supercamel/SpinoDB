#!/usr/bin/env sqgi
/*
 * Building native Spino documents from SQGI with DocNode.
 *
 * JSON strings are convenient, but DocNode lets you construct nested documents
 * without a serialize/parse round trip.
 */

local Spino = import("Spino", "1.2")

function line_item(sku, qty, unit_price) {
    local line = Spino.DocNode.new()
    line.set_object()
    line.add_string_member("sku", sku)
    line.add_uint_member("qty", qty)
    line.add_double_member("unit_price", unit_price)
    line.add_double_member("line_total", qty * unit_price)
    return line
}

function append_invoice(collection, invoice_id, customer, lines) {
    local invoice = Spino.DocNode.new()
    invoice.set_object()
    invoice.add_string_member("invoice_id", invoice_id)
    invoice.add_string_member("customer", customer)
    invoice.add_string_member("status", "issued")

    local total = 0.0
    local items = Spino.DocNode.new()
    items.set_array()

    foreach (line in lines) {
        total += line.line_total
        items.append(line_item(line.sku, line.qty, line.unit_price))
    }

    invoice.add_member("items", items)
    invoice.add_double_member("total", total)
    collection.append_node(invoice)
}

local db = Spino.Database.new()
local invoices = db.get_collection("invoices")
invoices.create_index("invoice_id")

append_invoice(invoices, "inv-1001", "Prahran Bakery", [
    { sku = "flour-25kg", qty = 4, unit_price = 32.80, line_total = 131.20 },
    { sku = "yeast-500g", qty = 10, unit_price = 6.10, line_total = 61.00 }
])

append_invoice(invoices, "inv-1002", "Carlton Coffee", [
    { sku = "cups-8oz", qty = 20, unit_price = 4.25, line_total = 85.00 },
    { sku = "lids-8oz", qty = 20, unit_price = 3.95, line_total = 79.00 },
    { sku = "napkins", qty = 8, unit_price = 5.40, line_total = 43.20 }
])

local view = invoices.find_one_view("{invoice_id:\"inv-1002\"}")
if (view == null) {
    throw "invoice not found"
}

print("invoice document:\n")
print(view.stringify() + "\n")

if (view.get_member_type("items") == Spino.VALUETYPE.array) {
    print("\nline items for " + view.get_string_member("invoice_id") + ":\n")

    local items = view.get_member("items")
    local iter = items.begin()
    local line_no = 1

    while (iter.is_not(items.end())) {
        local line = iter.get_view()
        print("  " + line_no + ". " + line.get_string_member("sku")
              + " x" + line.get_uint_member("qty")
              + " = $" + format("%.2f", line.get_double_member("line_total"))
              + "\n")
        line_no++
        iter.next()
    }

    print("total: $" + format("%.2f", view.get_double_member("total")) + "\n")
}
