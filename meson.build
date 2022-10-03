# project
project('clipboard', 'c',
        version        : '0.1.0_alpha6',
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

edje	= dependency('edje',		version: efl_version)
elm	= dependency('elementary',	version: efl_version)
e	= dependency('enlightenment',	version: e_version)

has_nls = get_option('nls')

host_os = host_machine.system()
if host_os == 'linux'
  host_os = 'linux-gnu'
endif

e_arch  = '@0@-@1@-@2@'.format(host_os, host_machine.cpu_family(), 
                               e.get_pkgconfig_variable('release'))
install_nls = true

# edje_cc binary compiler tool
edje_cmd = get_option('edje-cc')
if edje_cmd == ''
  edje_cmd = join_paths(edje.get_pkgconfig_variable('prefix'),
                        'bin', 'edje_cc')
endif

# dir locations
dir_home   = get_option('homedir-install')  
if dir_home == ''
	dir_prefix = join_paths(e.get_pkgconfig_variable('modules'), proj)
else
	dir_prefix = join_paths(dir_home, '.e/e/modules', proj)
	install_nls = false
endif
dir_mod    = join_paths(dir_prefix, e_arch)

# project args
add_project_arguments('-DHAVE_CONFIG_H=1', language : 'c')
if get_option('buildtype').startswith('debug')
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

# subdirs
subdir('data')
subdir('src')
if has_nls
  subdir('po')
endif
