#!/bin/bash

# Debug function
# $1:Debugger, $2: executable
debug() {
	echo "Debugging $2";
	$1 -ex "run" $2;
}


if [ $# -gt 0 ]
then
  file=$1
else
  file="wordament.c"
fi
out=${file:0:-2}

echo "Building $file ..."

if gcc -g -o $out $file; then
  echo 'Compiled successfully...';
	debug 'gdb' $out;
else
	echo 'Compile error, exiting...';
fi
