#!/bin/bash

main() {
	local bin_path="${1}"
	make clean

	CC="${bin_path}/wrapper_gcc" "${bin_path}/wrapper_make"

	python <<EOF
import json
with open('./compile_commands.json', 'r') as f:
    commands = json.load(f)
assert len(commands) == 1
object = commands[0]
assert object['file'] == 'test_case.c', 'Wrong file'
assert object['arguments'] == ['gcc', 'test_case.c', '-o', 'test_case.o', '-c'], 'Wrong arguments'
EOF
}

main "${@}"
