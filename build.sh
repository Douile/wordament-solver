#!/bin/bash

# Build function
# $1:Source files $2:Output file
build() {
	echo "Building $2";
	gcc -g -o $1 $2;
}

# Debug function
# $1:Debugger, $2: executable
debug() {
	echo "Debugging $2";
	$1 -ex "run" $2;
}


if [ $# -gt 0 ]; then
  files=$1;
else
  files="./src/board.c ./src/flags.c ./src/stack.c ./src/trie.c ./src/util.c ./src/wordament.c ./src/wordlist.c";
fi
out="wordament"

if build $out "$files"; then
  echo 'Compiled successfully...';
	debug 'gdb' $out;
else
	echo 'Compile error, exiting...';
fi
