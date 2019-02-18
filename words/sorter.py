#!/usr/bin/python3

import argparse

INPUT = 'words_alpha.txt'
OUTPUT = 'words_sorted.txt'

def check(line,**kwargs):
    # line of format: word\n
    # length constraints
    min_length = kwargs.get('min_length',3)
    max_length = kwargs.get('max_length',9)
    if not isinstance(line,str):
        if isinstance(line,bytes):
            line = str(line,'utf-8')
        elif isinstance(line,int) or isinstance(line,float):
            line = str(line)
        else:
            return False # unsupported type conversion
    length = len(line.strip())
    if length < min_length or length > max_length:
        return False
    return True

if __name__ == '__main__':
    parser = argparse.ArgumentParser(prog='Wordlist Sorter',description='Sort wordlists by custom search terms')
    parser.add_argument('-min','--min-length',type=int,default=3,dest='min_length')
    parser.add_argument('-max','--max-length',type=int,default=9,dest='max_length')
    parser.add_argument('-in','--input',type=str,default=INPUT,dest='input')
    parser.add_argument('-out','--output',type=str,default=OUTPUT,dest='output')
    args = parser.parse_args()

    input_file = open(args.input,'rb')
    output_file = open(args.output,'wb')

    print('Sorting file')
    input_file.seek(0,2)
    input_size = input_file.tell()
    input_file.seek(0)
    output_file.seek(0)

    try:
        while 1:
            line = input_file.readline()
            if check(line,min_length=args.min_length,max_length=args.max_length):
                output_file.write(line)
            p = input_file.tell()
            if p >= input_size:
                print('\nDone')
                break
            print('\r{0}/{1} [{2}%] bytes sorted...'.format(p,input_size,round(p/input_size*100)),end='',flush=True)
    except KeyboardInterrupt:
        print('\nCancelled')
    output_size = output_file.tell()
    print('Removed {0} [{1}%] bytes'.format(input_size-output_size,round(100-(output_size/input_size*100),2)))
    input_file.close()
    output_file.close()
