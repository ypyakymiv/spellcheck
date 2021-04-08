#!/bin/bash

for i in `seq 1 $1`
do
  ./test 10 0.5 1 &
  sleep 1s
done
