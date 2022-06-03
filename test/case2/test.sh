#!/bin/bash

main() {
	local bin_path="${1}"
	make clean

	CXX="${bin_path}/wrapper_g++" "${bin_path}/wrapper_make"

	python <<EOF
import json
with open('./compile_commands.json', 'r') as f:
    commands = json.load(f)
assert len(commands) == 1, 'Wrong size'
object = commands[0]
assert object['file'] == 'test_case.cc', 'Wrong file'
assert object['arguments'] == \
    ['g++', 'test_case.cc', '-g', '-std=c++17', '-DDEBUG', '-DMACRO="TEST TEST"', '-o', 'test_case.o', '-c'], 'Wrong arguments'
EOF
}

main "${@}"
