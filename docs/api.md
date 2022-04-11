# SpinoDB API Reference

SpinoDB has bindings for NodeJS and C/GObjects. GObject introspection automatically makes available bindings for a variety of languages such as Vala and Python. 

The only differences between the NodeJS and GObject bindings are

* NodeJS bindings use CamelCase whereas GObject bindings use snake_case. 
* NodeJS bindings can accept Javascript objects as documents for append() and update() whereas GObject bindings strictly use strings.


## Database

### Construction

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

### Persistence
#### save

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

#### load

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

#### enableJournal

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

#### consolidate

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

### 

## Collection

## Cursor


