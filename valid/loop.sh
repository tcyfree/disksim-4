#!/bin/sh
for i in `seq 1 $2`; do
  ../src/syssim $i 0 $1
done
