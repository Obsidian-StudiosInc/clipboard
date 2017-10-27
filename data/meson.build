configure_file(input  : 'module.desktop.in',
               output : 'module.desktop',
               install_dir : dir_prefix,
               configuration : cfg
)

cmd = [ edje_cmd,
	'-id', join_paths(meson.source_root(), 'data', 'images'),
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
