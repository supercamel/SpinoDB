# SpinoDB

SpinoDB is an in-memory NoSQL data library that is small, self-contained and with emphasis on speed. It is written in C++ and has bindings for both NodeJS and C using GObjects. Bindings for Vala, Python, Java, GJS (Gnome Javascript), Lua and so on are all automatically available through GObject Introspection.

This documentation will focus on NodeJS code, however all objects and functions are directly transferrable to C, Vala and Python. 

## Use Cases

### Web Services

SpinoDB integrates nicely with Node & Express. By switching from an enterprise grade solution to SpinoDB, small/medium websites might notice improvements in website responsiveness and lower server CPU useage.

Bots and online services that require fast document retrieval may find SpinoDB a more appropriate solution than a fully fledged enterprise system.

SpinoDB may also be used as a fast cache to complement a traditional database.

### Desktop Applications

Desktop applications can use SpinoDB as an alternative to creating an ad-hoc format for application specific data.

* application settings
* data logging
* application specific data

### Limitations

The practical limits are determined by

* how much RAM your system has. It is an in memory database.
* loading / saving time. If your system is reasonably spec'd you could budget ~100MB/s to load and save the database to a file.

As a rule of thumb, if the size of your data is greater than 50% of your available RAM, or if the save time is unmanageable, then this isn't for you.
