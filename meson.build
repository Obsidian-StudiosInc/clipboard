# project
project('clipboard', 'c',
        version        : '0.0.0_alpha1',
        license        : 'GPL-3.0',
        default_options: [ 'c_std=gnu99' ],
        meson_version  : '>= 0.43.0')
base_url = 'https://github.com/Obsidian-StudiosInc/'

# global arguments
add_global_arguments('-W', language : 'c')

# global variables
cc       = meson.get_compiler('c')
proj     = meson.project_name()
ver      = meson.project_version()
chmod    = join_paths(meson.source_root(), 'meson-chmod.sh')
top_inc  = include_directories('.')

# dependencies
e_version = '>= 0.21.0'
efl_version = '>= 1.19.0'

ecore_x	= dependency('ecore-x',		version: efl_version)
edje	= dependency('edje',		version: efl_version)
elm	= dependency('elementary',	version: efl_version)
e	= dependency('enlightenment',	version: e_version)

has_nls = get_option('nls')

e_arch  = '@0@-@1@-@2@'.format(host_machine.system(), 
                               host_machine.cpu_family(), 
                               e.get_pkgconfig_variable('release'))

# edje_cc binary compiler tool
edje_cmd = get_option('edje-cc')
if edje_cmd == ''
  edje_cmd = join_paths(edje.get_pkgconfig_variable('prefix'),
                        'bin', 'edje_cc')
endif

# dir locations
#dir_prefix = join_paths(get_option('homedir-install'), proj)
#if dir_prefix == ''
	dir_prefix = join_paths(e.get_pkgconfig_variable('modules'), proj)
#endif
dir_mod    = join_paths(dir_prefix, e_arch)

# project args
add_project_arguments('-DHAVE_CONFIG_H=1', language : 'c')
if get_option('debug')
	add_project_arguments('-g', language : 'c')
endif
if has_nls
	add_project_arguments('-DENABLE_NLS=1', language : 'c')
endif

# config.h
cfg = configuration_data()
cfg.set_quoted('PACKAGE'		, proj)
cfg.set_quoted('PACKAGE_NAME'		, proj)
cfg.set_quoted('PACKAGE_STRING'		, proj + ' ' + ver)
cfg.set_quoted('PACKAGE_URL'		, base_url + proj)
cfg.set_quoted('VERSION'		, ver)
configure_file(output: 'config.h', configuration: cfg)

configure_file(input  : 'module.desktop.in',
               output : 'module.desktop',
               install_dir : dir_prefix,
               configuration : cfg
)

# move to data
cmd = [ edje_cmd,
	'-id', join_paths(meson.source_root(), 'images'),
	'@INPUT@', '@OUTPUT@'
]
src = 'e-module-@0@'.format( proj )
custom_target(src + '.edj',
              input      : src + '.edc',
              output     : src + '.edj',
              command    : cmd,
              install_dir: dir_prefix,
              install    : true
             )
out = join_paths(dir_prefix, src + '.edj')
meson.add_install_script(chmod, 'a+r', out)

# subdirs
subdir('src')
if has_nls
  subdir('po')
endif
