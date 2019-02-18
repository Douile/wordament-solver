#!/bin/bash

if [ $# -gt 0 ]
then
  file=$1
else
  file="wordament.c"
fi
out=${file:0:-2}

echo "Building $file ..."

exec 5>&1
CC=$(gcc -o $out $file)
echo $CC
if [[ $CC = *"error"* ]] # this error check is not currently working
then
  echo "Error building, execution cancelled"
else
  echo "Successful build, starting in debugger"
  gdb -ex "run" $out
fi
