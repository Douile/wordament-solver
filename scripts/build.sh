#!/bin/bash

final_check() {
	if [ -x "$(command -v emcc)"]; then
		echo "Still couldn't find emcc in $PWD" >&2;
		exit 1;
	fi
}

# Check for required compiler
if [ -x "$(command -v emcc)"]; then
	echo "emcc not found in path, looking for emsdk source";
	if [ -f ./emsdk-cache/emsdk-master/emsdk_env.sh ]; then
		source ./emsdk-cache/emsdk-master/emsdk_env.sh;
		echo "Sourced emsdk, running check again";
		final_check;
	elif [ -f ./emsdk-cache/emsdk-master/emsdk ]; then
		echo "Found emsdk executable, constructing env";
		./emsdk-cache/emsdk-master/emsdk construct_env;
		final_check;
	else
		echo "Couldn't find emcc or emsdk in $PWD" >&2;
		exit 1;
	fi
fi

# Build function
# $1:Source files $2:Output file
build() {
	echo "Building $2";
	emcc -s WASM=1 -s EXTRA_EXPORTED_RUNTIME_METHODS="['cwrap']" -s ALLOW_MEMORY_GROWTH=1 -o $1 $2;
}

# Detect build files
if [ $# -gt 0 ]; then
  files=$1;
else
  files="./src/board.c ./src/flags.c ./src/stack.c ./src/trie.c ./src/util.c ./src/wordament.c ./src/wordlist.c ./src/web.c";
fi
out="./www/src/wordament.js";

# Build output
if build $out "$files"; then
  echo 'Compiled successfully...';
else
	echo 'Compile error, exiting...';
	exit 1;
fi
