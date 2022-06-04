# wrapper_c

[![ci_meson](https://github.com/adonis0147/wrapper_c/actions/workflows/ci_meson.yml/badge.svg)](https://github.com/adonis0147/wrapper_c/actions/workflows/ci_meson.yml)

_**WRAPPER_C**_ is a compiler wrapper. It is another [Clang's JSON compilation database](https://clang.llvm.org/docs/JSONCompilationDatabase.html) generator for **make-based** build systems.

Unlike [Bear](https://github.com/rizsotto/Bear) which uses **LD_PRELOAD**, wrapper_c exploits the implicit variables `CC` and `CXX` to intercept the compilation commands from `make` and forwards the commands to the compiler.

## Usage

```
Usage: ./wrapper_c [OPTION]... -- compiler

OPTION
        -h, --help      Help
        -v, --version   Version
        -n, --no_exec   Don't execute the command for the compiler.
        -d, --debug     Activate the logging.

```

## Installation

### Prerequisites

1. C++ compiler which supports C++ 17
2. Meson
3. Ninja
4. greadlink (MacOS < 12.0)

```shell
git clone https://github.com/adonis0147/wrapper_c
cd wrapper_c
git submodule update --init --progress
```

### Linux

```shell
meson build --buildtype=release -Dprefix=/path/to/wrapper_c
ninja install -C build
```

### MacOS

For MacOS < 12.0, `greadlink` should be installed.
```shell
brew install coreutils
alias readlink="$(which greadlink)"
```

Build from source.

```shell
meson build --buildtype=release -Dprefix=/path/to/wrapper_c
ninja install -C build
```

## User Manual

```shell
CC="/path/to/wrapper_c -- gcc" CXX="/path/to/wrapper_c -- g++" ./configure
/path/to/wrapper_make
```

For the sake of convenience, adding the path to the environment variable `PATH` is recommended.

### Alias

```shell
CC="/path/to/wrapper_gcc" CXX="/path/to/wrapper_g++" ./configure
/path/to/wrapper_make
```

### Parellel

```shell
/path/to/wrapper_make -j <num_jobs>
```
