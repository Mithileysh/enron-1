#!/usr/bin/python

import sys
import re

if len(sys.argv) < 3:
    sys.stderr.write("Usage: ./malletTopicToSVMLight.py" +
            " [input file] [output file]\n")
    sys.exit()

inputfile = open(sys.argv[1])
outputfile = open(sys.argv[2], 'w')

for line in inputfile:
    if line[0] == '#':
        continue

    tokens = line.rstrip('\n').rstrip('\t').split('\t')

    # extract target
    matchobj = re.match(r'.*/([^/]+)/[^/]+', tokens[1])
    target = matchobj.group(1)

    outputfile.write(target + ' ')

    isOddToken = True
    for i in range(2,len(tokens)):
        outputfile.write(tokens[i])

        if isOddToken:
            outputfile.write(':')
        else:
            outputfile.write(' ')

        isOddToken = not isOddToken

    outputfile.write('\n')

