inc = include_directories('.')

clipboard_srcs = [
	'clip_log.h',		'clip_log.c',
	'common.h',		'config_defaults.h',
	'e_mod_config.h',	'e_mod_config.c',
	'e_mod_main.h',		'e_mod_main.c',
	'history.h',		'history.c',
	'utility.h',		'utility.c'
]

shared_library('module',
	sources			: clipboard_srcs,
	include_directories	: [inc, top_inc],
	dependencies		: [edje, elm, e],
	install			: true,
	install_dir             : dir_mod,
	link_args		: '-Wl,--unresolved-symbols=ignore-all',
        name_prefix             : ''
)
