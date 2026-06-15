# SpinoDB SQGI Examples

These examples use SQGI through GObject Introspection, the same Spino API shape
used by the Python and Vala examples.

Run them with an installed `sqgi` and installed Spino typelib:

```sh
sqgi examples/sqgi/basic.nut
```

When running against the local build tree, point SQGI at the generated Spino
typelib and shared library:

```sh
GI_TYPELIB_PATH=builddir LD_LIBRARY_PATH=builddir sqgi examples/sqgi/basic.nut
```

Examples:

- `basic.nut` - order workflow: append JSON tables, query, update, drop, save/load.
- `queries.nut` - query operator tour using incident records.
- `docnode.nut` - native `Spino.DocNode` construction and `DocView` traversal.
- `persistence.nut` - save/load, key-value metadata, and journal recovery.
- `inventory_report.nut` - cursor-driven report over warehouse stock data.
- `benchmark.nut` - small indexed lookup timing example.
