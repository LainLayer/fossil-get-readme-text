#!/usr/bin/env sh

set -xe

gcc -o main main.c -Wall -Wextra -Werror -pedantic -lsqlite3 -lz
