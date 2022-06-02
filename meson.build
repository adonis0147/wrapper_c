project('wrapper_c', 'cpp', version : 'v0.1')

subdir('src')

compiler = meson.get_compiler('cpp').get_id()
build_type = get_option('buildtype')

includes = include_directories('include')

flags = ['-DVERSION="@0@"'.format(meson.project_version()), '-DBUILD_TYPE="@0@"'.format(build_type)]
flags += ['-std=c++17', '-g']
flags += ['-Wall', '-Wextra']

link_flags = []

if (build_type.startswith('debug') and compiler == 'gcc')
    flags += ['-fsanitize=address', '-fno-omit-frame-pointer']
    link_flags += ['-Wl,-Bstatic', '-lasan', '-Wl,-Bdynamic']
endif

executable(
    meson.project_name(), srcs,
    include_directories: includes,
    cpp_args : flags,
    link_args: link_flags,
    install: true
)

install_data(
    [
      'scripts/wrapper_make',
      'scripts/wrapper_gcc',
      'scripts/wrapper_g++'
    ],
    install_dir: 'bin'
)
