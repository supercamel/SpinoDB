{
    "targets": [
        {
            "target_name": "spinodb",
            "sources": [
                "cppsrc/query_executor.cpp",
                "cppsrc/query_parse.cpp",
                "cppsrc/collection.cpp",
                "cppsrc/database.cpp",
                "cppsrc/dom_node.cpp",
                "cppsrc/dom_obj.cpp",
                "cppsrc/dom_arr.cpp",
                "cppsrc/spino_squirrel.cpp",
                "spino_node/SpinoWrapper.cpp",
                "spino_node/CollectionWrapper.cpp",
                "spino_node/CursorWrapper.cpp",
                "spino_node/main.cpp",
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
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-rtti", "-fno-exceptions"],
            "cflags": ["-O3"],
            "cflags_cc": ["-O3"],
            'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")", "cppsrc", "cppsrc/squirrel/include"],
            'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
            "conditions": [
                [
                    "OS==\"mac\"", {
                        "xcode_settings": {
                            "OTHER_CFLAGS": [
                                "-mmacosx-version-min=10.7",
                                "-std=c++14",
                                "-stdlib=libc++"
                            ],
                            "GCC_ENABLE_CPP_RTTI": "YES",
                            "GCC_ENABLE_CPP_EXCEPTIONS": "YES"
                        }
                    }
                ]
            ]
        }
    ]
}
