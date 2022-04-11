# Cursor Scripts

The Cursors returned from find queries can execute Squirrel scripts to further refine search results. Cursor scripting can be used to

* order the search results
* format the query result into a particular format (e.g. CSV or XML)
* aggregation and process query results within SpinoDB
* write data to files

The benefits of using cursor scripts over processing results in 'native' code are

* Speed. Cursor scripts generally process results faster than Javascript and C/Vala.
* Simplicity. Mostly to the benefit of C/Vala users, cursor scripts provide a very concise way of processing query results.

[Squirrel language documentation](http://www.squirrel-lang.org/squirreldoc/reference/language.html)

Cursor scripts must contain two functions called 'result' and 'finalize'.

The result function is called iteratively for each document that the query matches. The document is passed as a 'table' to the 'result' function. Once the cursor has no more matching documents, the 'finalize' function will be called. The finalize function does not take any parameters but it must return a result. The result is returned as a string to the application. If the result is a table or array, it will be stringified into JSON.

```
csv <- "";
function result(doc) {
    foreach(key, value in doc) {
        csv += value.tostring() + ",";
    }
    csv += "\n";
}

function finalize() {
    return csv;
}
```

The above example will dump all values in the documents to a CSV formatted string.

Cursor scripting is a very powerful and flexible way to deal with the data, but it's much better and more efficient to use the query language and projections whenever possible, even just to whittle down the results.

## Running Scripts

```
var oldest_trains = db.get_collection("trains").
                    find("{}").
                    projection("{\"name\": 1, \"year\": 1}").
            run_script("""

trains <- [];
function result(r) {
    if(trains.len() < 5) {
        trains.append(r);
        trains.sort(@(a,b) a.year <=> b.year);
    }
    else {
        if(r.year < trains[4].year) {
            trains[4] = r;
            trains.sort(@(a,b) a.year <=> b.year);
    }
    }
}

function finalize() {
    return trains;
}
""");
```

This Vala example starts by retrieving every document in the 'trains' collection and setting a projection to only retrieve the 'name' and 'year' fields. The script then keeps a track of only the 5 oldest trains and sorts them by the 'year' field. The returned value of the finalize function, which is an ordered list of the 5 oldest trains, is stringified and assigned to the 'oldest_trains' variable.
Notes on script performance

Cursor scripts are generally more efficient for processing large numbers of results than repeatedly calling cursor.next() in your application. With the 'solarsystem' test case, the cursor script executes about 30% faster than equivalent NodeJS javascript and 65% faster than equivalent Vala. Cursor scripts are fast because the documents processed by cursor scripts are not converted to and from strings. They are converted from Spino's internal DOM memory format directly to a table in the Squirrel VM which is a fairly efficient operation. On the other hand, processing results by calling cursor.next() will make Spino convert the document into a string. Most likely, the application will parse the document back into an object/DOM.

Cursor scripts 

    Spino DOM -> Squirrel table

Processing cursor results in 'native' language 

    Spino DOM -> stringified -> parsed -> 'native' dom/table

The Json-Glib parser seems particularly slow and is the reason why Vala (a compiled language) runs slower than Javascript when processing query results.
