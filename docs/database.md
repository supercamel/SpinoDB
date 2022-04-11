# Database

SpinoDB has bindings for NodeJS and C/GObjects. GObject introspection automatically makes available bindings for a variety of languages such as Vala and Python. 

The only differences between the NodeJS and GObject bindings are

* NodeJS bindings use CamelCase whereas GObject bindings use snake_case. 
* NodeJS bindings can accept Javascript objects as documents for append() and update() whereas GObject bindings strictly use strings.



## Construction

NodeJS 

```
const spino = require('spinodb');
var db = new spino.Spino(); // create the db instance
```

C

```
#include <Spino-1.0.h>

int main() {
    SpinoDatabase* db = spino_database_new();
}
```

## Persistence
### save

Dumps the database to a JSON file.

    db.save(<path name>)

**Parameters**

none

**Returns**

no return value

**Example**

NodeJS
```
db.save("data.db");
```

C
```
spino_database_save(db, "data.db");
```

### load

Loads the database from a JSON file. Returns true on success or false on failure.

    db.load(<path>)

**Parameters**

1. The path to the file to load

**Returns** 

true on success, false on failure

**Example**

NodeJS
```
if(db.load("data.db")) { console.log("loaded ok"); }
```

C
```
if(spino_database_load(db, "data.db") == FALSE) {
    printf("error loading data.db\n");
}
```

### enableJournal

Journalling makes Spino record every change to the data to a journal file. It's fast to append data to a file, so this doesn't create an appreciable dent in performance but it almost gaurantees that data won't be lost after a crash. 

    enableJournal(<journal file name>)

**Parameters**

1. The name of the file to write the journal to

**Returns**

No return value

**Example**

NodeJS
```
db.enableJournal("journal");
```

C
```
spino_database_enable_journal(db, "journal");
```

### consolidate

The consolidate function loads the journal file and replays it against the database, then saves the data to the specified file name. Typically, this would be done immediately after the call the 'enableJournal'.

Note this function will use the journal file name provided to the 'enableJournal' function. enableJournal() must be called prior to calling consolidate().

    db.consolidate(<database file name>);

**Parameters**

1. The file name to save the database to

**Returns**

No return value

**Example**

NodeJS
```
db.consolidate("data.db");
```

C
```
spino_database_consolidate(db, "data.db");
```

## Collections

### getCollection

getCollection will get a reference to the collection. If the collection does not exist, it will be created. 

**Parameters**

1. The name of the collection

**Returns**

The collection object

**Example**

NodeJS

```
var col = db.getCollection("students");
```

C
```
SpinoCollection* col = spino_database_get_collection(db, "students");
```

### dropCollection

dropCollection will delete the collection and all of its documents. If the collection does not exist, this function will do nothing.

**Parameters**

1. The name of the collection to delete.

**Returns**

No return value

**Example**

NodeJS
```
db.dropCollection("students");
```

C
```
spino_database_drop_collection("students");
```


## Key Store

### setIntValue

setIntValue sets an integer value in the key store. If the key already exists it will be overwritten.

**Parameters**

1. The key name as a string
2. The integer value to store

**Returns**

No return value

## setUintValue

setUintValue sets a unsigned integer value in the key store. If they key already exists it will be overwritten.

**Parameters**

1. The key name as a string
2. The value as an unsigned integer

**Returns**

No return value

## setDoubleValue

setDoubleValue sets a double precision floating value in the key store. Overwrites any existing entry.

**Parameters**

1. The key name as a string
2. The value as a double

**Returns**

No return value


## setStringValue

setStringValue sets a string in the key store. Overwrites existing entry.

**Parameters**

1. The key name as a string
2. The value as a string

**Returns**

No return value

## setBoolValue

setBoolValue sets a boolean value in the key store.

**Parameters**

1. The key name as a string
2. The value as a boolean

**Returns**

No return value

## getIntValue

getIntValue gets an integer value from the key store.

**Parameters**

1. The key name as a string

**Returns**

The value as an integer, or 0 if the key does not exist, or the value is not an integer type.

## getUintValue

getUintValue gets an unsigned integer from the key store.

**Parameters**

1. The key name as a string

**Returns**

The value as an unsigned integer, or 0 if the key does not exist or the value is of the wrong type.


## getDoubleValue

getDoubleValue gets a double value from the key store.

**Parameters**

1. The key name as a string

**Returns**

The value as a double, or 0 if the key does not exist or the value is of the wrong type.

## getStringValue

getStringValue gets a string from the key store.

**Parameters**

1. The key name as a string

**Returns**

The value as a string, or an empty string if the key does not exist or is of the wrong type.

## getBoolValue

getBoolValue gets a boolean from the key store

**Parameters**

1. The key name as a string

**Return**

The boolean value, or false if the key does not exist or is of the wrong type.


