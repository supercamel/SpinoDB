# SpinoDB

SpinoDB is an in-memory NoSQL data library that is small, self-contained and with emphasis on **speed**. It is written in C++ and has bindings for both NodeJS and C using GObjects. Bindings for Vala, Python, Java, GJS (Gnome Javascript), Lua and so on are all automatically available through GObject Introspection. 

Be aware that code in the repo is often broken and buggy. The repo is real-time work in progress. Check out releases for a more stable version.

### Documentation

https://spinodb.readthedocs.io/en/latest/


### Installation

## NodeJS 

    npm install spinodb

## GObject

    meson --prefix=/usr builddir
    ninja -C builddir
    cd builddir && sudo meson install

This will build and install the libary along with pkg-config, gir, vapi and typelib files.
