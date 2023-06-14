# SpinoDB

SpinoDB is an in-memory NoSQL data library that is small, self-contained and with emphasis on **speed**. It is written in C++ and has bindings for both NodeJS and C using GObjects. Bindings for Vala, Python, Java, GJS (Gnome Javascript), Lua and so on are all automatically available through GObject Introspection. 


### Documentation

https://spinodb.readthedocs.io/en/latest/


### Installation

## NodeJS 

```
npm install spinodb
```

## GObject

Install meson & ninja to build the project
```
sudo pip3 install meson
sudo pip3 install ninja
```

Install gobject introspection dev package
```
apt install libgirepository1.0-dev
```

Build and install
```
meson builddir
cd builddir && sudo meson install
```

This will build and install the libary along with pkg-config, gir, vapi and typelib files.
