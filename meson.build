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
    'cppsrc/QueryExecutor.cpp',
    'cppsrc/QueryParser.cpp',
    'cppsrc/SpinoDB.cpp',
    'cppsrc/Cursor.cpp',
    'cppsrc/Collection.cpp'
]

spinolib= shared_library('spino-1.0',
		spino_sources,
        	vala_header: 'spino_gobject/inc/spino.h',
		vala_vapi: 'spino-1.0.vapi',
		vala_gir: 'spino-1.0.gir',
		include_directories : 'spino_gobject/inc',
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
  'spino_gobject/inc/cursor.h'
  ]

install_headers(headers, subdir: 'spino')

introspection_sources= [
	'spino_gobject/src/cursor.cpp',
	'spino_gobject/src/database.cpp',
	'spino_gobject/src/collection.cpp',
    'spino_gobject/src/spino.cpp',
	'spino_gobject/inc/cursor.h',
	'spino_gobject/inc/database.h',
	'spino_gobject/inc/collection.h',
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

