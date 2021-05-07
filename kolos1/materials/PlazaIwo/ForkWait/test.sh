#!/bin/bash

fib() {
    ./main $1
    echo $?
}

test_fib() {
    n=$1
    correct=$2
    val=$(fib $n)
    a=$( ((val == $correct)) && echo "ok" || echo "ERROR, should be $correct")
    echo "Fib($n) = $val - $a"
}

test_fib 0 1
test_fib 1 1
test_fib 2 2
test_fib 3 3
test_fib 4 5
test_fib 5 8
test_fib 6 13
test_fib 7 21
test_fib 8 34
test_fib 9 55
test_fib 10 89
test_fib 11 144
