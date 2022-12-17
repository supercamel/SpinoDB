# Getting Started

## NodeJS

    npm install spinodb

Alternatively, clone the git repo and use npm link

```
git clone https://github.com/supercamel/SpinoDB
cd SpinoDB
node-gyp configure && node-gyp build
npm link
```

In your project directory

    npm link spinodb


## GObject

Clone the git repo

    git clone https://github.com/supercamel/SpinoDB

Change into the SpinoDB directory
    
    cd SpinoDB

Build SpinoDB using meson

```
meson --prefix=/usr builddir
ninja -C builddir
cd builddir && sudo meson install
```

This will build and install the libary along with pkg-config, gir, vapi and typelib files required for GObject introspection bindings. 

## Examples

```
const spino = require('spinodb');

// create a spino database
var sdb = new spino.Spino();

// getCollection will create a collection if it doesn't exist
var col = sdb.getCollection("myCollection");

// using the 'count' field as the index for this collection
col.createIndex("count");

// add some documents
for(var i = 0; i < 10; i++) {
    col.append({count: i});
}

// find the document where count is 5
var result = col.findOne("{count:5}");
resultObj = JSON.parse(resultObj);
```


Refer to the examples directory on GitHub

[https://github.com/supercamel/SpinoDB/tree/master/examples](https://github.com/supercamel/SpinoDB/tree/master/examples)
