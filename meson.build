project('wrapper_c', 'cpp', version : '0.1', default_options : ['cpp_std=c++20'])

subdir('src')

includes = include_directories('include')
cflags = ['-g', '--std=c++20', '-Wall']

executable('wrapper_c', srcs, include_directories: includes, c_args: cflags)
