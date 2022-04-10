{
    "targets": [
    {
        "target_name": "spinodb",
            "sources": [ 
                "cppsrc/main.cpp",
            "cppsrc/QueryExecutor.cpp",
            "cppsrc/QueryParser.cpp",
            "cppsrc/IndexResolver.cpp",
            "cppsrc/SpinoDB.cpp",
            "cppsrc/Cursor.cpp",
            "cppsrc/Collection.cpp",
            "cppsrc/SpinoSquirrel.cpp",
            "cppsrc/SpinoWrapper.cpp",
            "cppsrc/Journal.cpp",
            "cppsrc/squirrel/squirrel/sqapi.cpp",
            "cppsrc/squirrel/squirrel/sqbaselib.cpp",
            "cppsrc/squirrel/squirrel/sqclass.cpp",
            "cppsrc/squirrel/squirrel/sqcompiler.cpp",
            "cppsrc/squirrel/squirrel/sqdebug.cpp",
            "cppsrc/squirrel/squirrel/sqfuncstate.cpp",
            "cppsrc/squirrel/squirrel/sqlexer.cpp",
            "cppsrc/squirrel/squirrel/sqmem.cpp",
            "cppsrc/squirrel/squirrel/sqobject.cpp",
            "cppsrc/squirrel/squirrel/sqstate.cpp",
            "cppsrc/squirrel/squirrel/sqtable.cpp",
            "cppsrc/squirrel/squirrel/sqvm.cpp",
            "cppsrc/squirrel/sqstdlib/sqstdaux.cpp",
            "cppsrc/squirrel/sqstdlib/sqstdblob.cpp",
            "cppsrc/squirrel/sqstdlib/sqstdmath.cpp",
            "cppsrc/squirrel/sqstdlib/sqstdrex.cpp",
            "cppsrc/squirrel/sqstdlib/sqstdstring.cpp",
            "cppsrc/squirrel/sqstdlib/sqstdstream.cpp",
            "cppsrc/squirrel/sqstdlib/sqstdio.cpp",
            "cppsrc/squirrel/sqstdlib/sqstdsystem.cpp"

                ],
                "cflags!": [ "-fno-exceptions" ],
                "cflags_cc!": [ "-fno-exceptions" ],
                "cflags": [ "-O3" ],
                "cflags_cc": ["-O3"],
                "include_dirs": ["cppsrc/squirrel/include"]
    }
    ]
}
