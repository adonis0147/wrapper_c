project('wrapper_c', 'cpp', version : 'v0.1')

subdir('src')

compiler = meson.get_compiler('cpp').get_id()
build_type = get_option('buildtype')

command = run_command('date', check: false)
if command.returncode() == 0
    compiled_time = command.stdout().strip()
else
    compiled_time = ''
endif
message('compiled time: ' + compiled_time)

command = run_command('git', 'show', '--format=%H', '--no-patch', check: false)
if command.returncode() == 0
    commit_id = command.stdout().strip()
else
    commit_id = ''
endif
message('commit id: ' + commit_id)

includes = include_directories('include')

flags = [
    '-DVERSION="@0@"'.format(meson.project_version()),
    '-DBUILD_TYPE="@0@"'.format(build_type),
    '-DCOMPILED_TIME="@0@"'.format(compiled_time),
    '-DCOMMIT_ID="@0@"'.format(commit_id),
]
flags += ['-std=c++17', '-g']
flags += ['-Werror', '-Wall', '-Wextra', '-Wpedantic']

link_flags = []

if host_machine.system() == 'linux'
    if build_type.startswith('debug')
        flags += ['-fsanitize=address', '-fno-omit-frame-pointer']
        link_flags += ['-Wl,-Bstatic', '-lasan', '-Wl,-Bdynamic']
    elif build_type.startswith('release')
        link_flags += ['-static']
    endif
endif

executable(
    meson.project_name(),
    srcs,
    include_directories: includes,
    cpp_args: flags,
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
