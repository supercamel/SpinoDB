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
                "cflags!": [ "-fno-exceptions", "-std=c++20" ],
                "cflags_cc!": [ "-fno-rtti","-fno-exceptions", "-std=c++20" ],
                "cflags": [ "-O3" , "-std=c++20"],
                "cflags_cc": ["-O3", "-std=c++20"],
                "include_dirs": ["cppsrc/squirrel/include"],
                   'xcode_settings': {
                        'OTHER_CFLAGS': [
                          "-std=c++20",
                          "-stdlib=libc++"
                        ],
                      },
                "conditions": [
                        ["OS=='linux'", {

                          "ldflags": [
                            "-fPIC",
                            "-fvisibility=hidden"
                          ],
                          "cflags": [
                            "-fPIC",
                            "-fvisibility=hidden","-std=c++20", "-O3", "-msse4.2", "-march=native"
                          ],
                          "cflags_cc": [
                            "-fPIC",
                            "-fvisibility=hidden",
                            "-fvisibility-inlines-hidden","-std=c++20", "-O3", "-msse4.2", "-march=native"
                          ]
                        }],
                        [
                          "OS==\"mac\"", {
                            "xcode_settings": {
                              "OTHER_CFLAGS": [
                                "-mmacosx-version-min=10.7",
                                "-std=c++20",
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
