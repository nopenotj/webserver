#!/usr/bin/env sh

if [ "$1" = "run" ]; then
    gcc -g main.c ./logger/logger.c ./array/array.c && ./a.out
elif [ "$1" = "test" ]; then
    gcc -g test.c ./array/array.c && ./a.out
fi