{
    "targets": [{
        "target_name": "spinodb",
        "cflags!": [ "-fno-exceptions" ],
        "cflags_cc!": [ "-fno-exceptions" ],
        "sources": [
            "cppsrc/main.cpp",
			"cppsrc/SpinoDB.cpp",
			"cppsrc/QueryExecutor.cpp",
			"cppsrc/QueryParser.cpp",
			"cppsrc/SpinoWrapper.cpp"
        ],
        'include_dirs': [
            "<!@(node -p \"require('node-addon-api').include\")",
			"cppsrc"
        ],
        'libraries': [],
        'dependencies': [
            "<!(node -p \"require('node-addon-api').gyp\")"
        ],
        'defines': [ 'NAPI_DISABLE_CPP_EXCEPTIONS' ]
    }]
}
