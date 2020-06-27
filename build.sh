#!/bin/bash

# Build function
# $1:Source files $2:Output file
build() {
	echo "Building $2";
	emcc -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS="['cwrap']" -s ALLOW_MEMORY_GROWTH=1 -o $1 $2;
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
  files="./src/board.c ./src/flags.c ./src/stack.c ./src/trie.c ./src/util.c ./src/wordament.c ./src/wordlist.c ./src/web.c";
fi
out="./www/wordament.js"

if build $out "$files"; then
  echo 'Compiled successfully...';
else
	echo 'Compile error, exiting...';
fi
