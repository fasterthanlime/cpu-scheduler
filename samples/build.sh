#!/bin/bash

# Yeah, there's no make in the UML image.. bummer

for i in *.c; do
    j=$(basename $i .c)
    echo "Compiling $i"
    gcc -o $j $i
done
