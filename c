#!/usr/bin/env sh

deps="./logger/logger.c ./array/array.c ./dict/dict.c"
if [ "$1" = "run" ]; then
    gcc -g main.c  $deps && ./a.out
elif [ "$1" = "test" ]; then
    gcc -g $deps ./test/main.c  -o test.out && ./test.out
fi
