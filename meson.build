project('spino-1.2', 'c', 'cpp', 'vala',
  version: '0.1.2',
  license: 'MIT',
  default_options: ['cpp_std=c++17',  'buildtype=release'],
)

gnome = import('gnome')
pkg = import('pkgconfig')

dependencies = [
  dependency('glib-2.0'),
  dependency('gobject-2.0')
  ]

pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : 'libspino',
  version : '1.2',
  name : 'libspino',
  filebase : 'libspino',
  description : 'GObject wrapper for SpinoDB')

spino_sources= [
  'cppsrc/query_executor.cpp',
  'cppsrc/query_parse.cpp',
  'cppsrc/database.cpp',
  'cppsrc/dom_node.cpp',
  'cppsrc/collection.cpp',
  'cppsrc/dom_arr.cpp',
  'cppsrc/dom_obj.cpp',
  'cppsrc/dom_node.cpp',
  'spino_gobject/src/database.cpp',
  'spino_gobject/src/collection.cpp',
  'spino_gobject/src/cursor.cpp', 
  'spino_gobject/src/spino.cpp'
  ]

spinolib= shared_library('spino-1.2',
  spino_sources,
  cpp_args: ['-O3'],
  c_args: ['-O3'],
  vala_header: 'spino_gobject/inc/spino.h',
  vala_vapi: 'spino-1.2.vapi',
  vala_gir: 'spino-1.2.gir',
  include_directories : ['spino_gobject/inc'],
  dependencies : dependencies,
  install : true
  )

pkg.generate(spinolib,
  subdirs: 'spino'
  )

headers = [
  'spino_gobject/inc/Spino-1.2.h',
  'spino_gobject/inc/database.h',
  'spino_gobject/inc/collection.h',
  'spino_gobject/inc/cursor.h'
  ]

install_headers(headers, subdir: 'spino')

introspection_sources= [
  'spino_gobject/src/database.cpp',
  'spino_gobject/src/spino.cpp',
  'spino_gobject/inc/cursor.h',
  'spino_gobject/inc/database.h',
  'spino_gobject/inc/collection.h',
  'spino_gobject/inc/Spino-1.2.h'
  ]

g_ir_compiler = find_program('g-ir-compiler')
spino_gir = gnome.generate_gir(spinolib,
  sources : introspection_sources,
  nsversion : '1.2',
  namespace : 'Spino',
  symbol_prefix : 'spino_',
  identifier_prefix : 'Spino',
  export_packages : 'spino',
  includes : ['GObject-2.0'],
  install : true,
  install_dir_gir: get_option('libdir') / 'girepository-1.0',
  install_dir_typelib: get_option('libdir') / 'girepository-1.0'
  )[0]

gnome.generate_vapi('spino-1.2',
  install: true,
  sources: spino_gir
  )

