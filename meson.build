project('spino-1.0', 'c', 'cpp', 'vala',
  version: '0.1.0',
  license: 'MIT')

gnome = import('gnome')
pkg = import('pkgconfig')

dependencies = [
  dependency('glib-2.0'),
  dependency('gobject-2.0')
  ]

pkg_mod = import('pkgconfig')
pkg_mod.generate(libraries : 'libspino',
  version : '1.0',
  name : 'libspino',
  filebase : 'libspino',
  description : 'GObject wrapper for SpinoDB')

spino_sources= [
  'spino_gobject/src/cursor.cpp',
  'spino_gobject/src/database.cpp',
  'spino_gobject/src/collection.cpp',
  'spino_gobject/src/spino.cpp',
  'spino_gobject/src/value.cpp',
  'spino_gobject/src/document_viewer.cpp',
  'cppsrc/QueryExecutor.cpp',
  'cppsrc/QueryParser.cpp',
  'cppsrc/IndexResolver.cpp',
  'cppsrc/SpinoDB.cpp',
  'cppsrc/Cursor.cpp',
  'cppsrc/Collection.cpp',
  'cppsrc/SpinoSquirrel.cpp',
  'cppsrc/Journal.cpp',
  'cppsrc/squirrel/squirrel/sqapi.cpp',
  'cppsrc/squirrel/squirrel/sqbaselib.cpp',
  'cppsrc/squirrel/squirrel/sqclass.cpp',
  'cppsrc/squirrel/squirrel/sqcompiler.cpp',
  'cppsrc/squirrel/squirrel/sqdebug.cpp',
  'cppsrc/squirrel/squirrel/sqfuncstate.cpp',
  'cppsrc/squirrel/squirrel/sqlexer.cpp',
  'cppsrc/squirrel/squirrel/sqmem.cpp',
  'cppsrc/squirrel/squirrel/sqobject.cpp',
  'cppsrc/squirrel/squirrel/sqstate.cpp',
  'cppsrc/squirrel/squirrel/sqtable.cpp',
  'cppsrc/squirrel/squirrel/sqvm.cpp',
  'cppsrc/squirrel/sqstdlib/sqstdaux.cpp',
  'cppsrc/squirrel/sqstdlib/sqstdblob.cpp',
  'cppsrc/squirrel/sqstdlib/sqstdmath.cpp',
  'cppsrc/squirrel/sqstdlib/sqstdrex.cpp',
  'cppsrc/squirrel/sqstdlib/sqstdstring.cpp',
  'cppsrc/squirrel/sqstdlib/sqstdstream.cpp',
  'cppsrc/squirrel/sqstdlib/sqstdio.cpp',
  'cppsrc/squirrel/sqstdlib/sqstdsystem.cpp'
  ]

spinolib= shared_library('spino-1.0',
  spino_sources,
  cpp_args: ['-O3'],
  c_args: ['-O3'],
  vala_header: 'spino_gobject/inc/spino.h',
  vala_vapi: 'spino-1.0.vapi',
  vala_gir: 'spino-1.0.gir',
  include_directories : ['spino_gobject/inc', 'cppsrc/squirrel/include'],
  dependencies : dependencies,
  install : true
  )

pkg.generate(spinolib,
  subdirs: 'spino'
  )

headers = [
  'spino_gobject/inc/Spino-1.0.h',
  'spino_gobject/inc/database.h',
  'spino_gobject/inc/collection.h',
  'spino_gobject/inc/cursor.h',
  'spino_gobject/inc/value.h',
  'spino_gobject/inc/document_viewer.h'
  ]

install_headers(headers, subdir: 'spino')

introspection_sources= [
  'spino_gobject/src/cursor.cpp',
  'spino_gobject/src/database.cpp',
  'spino_gobject/src/collection.cpp',
  'spino_gobject/src/spino.cpp',
  'spino_gobject/src/value.cpp',
  'spino_gobject/src/document_viewer.cpp',
  'spino_gobject/inc/cursor.h',
  'spino_gobject/inc/database.h',
  'spino_gobject/inc/collection.h',
  'spino_gobject/inc/value.h',
  'spino_gobject/inc/document_viewer.h',
  'spino_gobject/inc/Spino-1.0.h'
  ]

g_ir_compiler = find_program('g-ir-compiler')
spino_gir = gnome.generate_gir(spinolib,
  sources : introspection_sources,
  nsversion : '1.0',
  namespace : 'Spino',
  symbol_prefix : 'spino_',
  identifier_prefix : 'Spino',
  export_packages : 'spino',
  includes : ['GObject-2.0'],
  install : true,
  install_dir_gir: get_option('libdir') / 'girepository-1.0',
  install_dir_typelib: get_option('libdir') / 'girepository-1.0'
  )[0]

gnome.generate_vapi('spino-1.0',
  install: true,
  sources: spino_gir
  )

