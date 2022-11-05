{
    "targets": [
    {
        "target_name": "spinodb",
            "sources": [ 
                "cppsrc/main.cpp",
                "cppsrc/query_executor.cpp",
                "cppsrc/query_parse.cpp",
                "cppsrc/collection.cpp",
                "cppsrc/database.cpp",
                "cppsrc/dom_node.cpp",
                "cppsrc/dom_obj.cpp",
                "cppsrc/dom_arr.cpp",
                "cppsrc/SpinoWrapper.cpp"
                ],
                "cflags!": [ "-fno-exceptions" ],
                "cflags_cc!": [ "-fno-rtti","-fno-exceptions" ],
                "cflags": [ "-O3" ],
                "cflags_cc": ["-O3"],
                "include_dirs": ["cppsrc/squirrel/include"],
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
