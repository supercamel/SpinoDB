project('spino_test', 'vala')

dependencies = [
  dependency('glib-2.0'),
  dependency('gobject-2.0'),
  dependency('spino-1.0'),
  dependency('json-glib-1.0'),
  meson.get_compiler('c').find_library('m', required: false)
  ]

sources = files('main.vala')

executable('spino_test', sources, c_args:['-O3'], dependencies: dependencies)


