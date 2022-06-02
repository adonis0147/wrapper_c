project('wrapper_c', 'cpp', version : 'v0.1')

subdir('src')

build_type = get_option('buildtype')

includes = include_directories('include')

flags = ['-DVERSION="@0@"'.format(meson.project_version()), '-DBUILD_TYPE="@0@"'.format(build_type)]
flags += ['-std=c++17', '-g']
flags += ['-Wall', '-Wextra']

link_flags = []

if (build_type.startswith('debug'))
    flags += ['-fsanitize=address']
    link_flags += ['-fsanitize=address']
endif

executable('wrapper_c', srcs, include_directories: includes, cpp_args : flags, link_args: link_flags)
