#!/bin/sh
for i in `seq 1 $1`; do
  ../src/syssim $i 0 $2
done
