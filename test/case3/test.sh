#!/bin/bash

set -e

main() {
	local bin_path="${1}"
	make clean

	CC="${bin_path}/wrapper_gcc" "${bin_path}/wrapper_make"

	python <<EOF
import json
with open('./compile_commands.json', 'r') as f:
    commands = json.load(f)

assert len(commands) == 2, 'Wrong size'

object = commands[0]
assert object['file'] == 'test_case.c', 'Wrong file'
assert object['arguments'] == ['gcc', 'test_case.c', '-MM', \
    '-MF', 'test_case.o.d', '-MT', 'test_case.o', '-o', 'test_case.o', '-c'], 'Wrong arguments'

object = commands[1]
assert object['file'] == 'test_case.c', 'Wrong file'
assert object['arguments'] == ['gcc', 'test_case.c', '-MM', \
    '-MF', 'test_case.oo.d', '-MQ', 'test_case.oo', '-o', 'test_case.o', '-c'], 'Wrong arguments'
EOF
}

main "${@}"
