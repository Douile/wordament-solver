# wordament-solver
Solves wordament puzzles using a Trie search (WIP).

_This an attempt to use lib [curses](https://en.wikipedia.org/wiki/Curses_%28programming\_library%29) to provide better usability_

## Usage
Compile using `gcc -o wordament wordament.c` (**warning**: compiled binary must be run with relative file `./words/words_3\_9.txt` accessible)

### Additional scripts
On linux you can compile and immediately run for debugging in gdb using `./build.sh`

You can sort wordlists using `./words/sorter.py` the arguments it accepts can be shown using `./words/sorter.py --help`


## Requirements
Your compiler must fill: `<stdio.h>`, `<stdlib.h>`, `<string.h>`

## Changelog
**v.01** - Mostly working but segmentation issues with add_word (storing words in linked list after they have been found)

[License](https://github.com/Douile/wordament-solver/blob/master/LICENSE)
