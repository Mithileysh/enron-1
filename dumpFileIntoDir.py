#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import json
import os
import re

# error check
if len(sys.argv) < 2:
    sys.stderr.write("USUAGE: ./dumpFileIntoDir.py [out directory]\n")
    sys.exit(1)

root = ''.join([sys.argv[1], '/'])
if os.path.exists(root):
    sys.stderr.write("%s already exists\n" % root)
    sys.exit(1)

# read file into json object
with open('top5people-sentbox.json') as f:
    doc = json.loads(f.read())

"""Main Block
Usage: ./dumpFileIntoDir.py [output directory]

This code reads the dataset from the file and dump the contents into directories
in the directory given as an argument. Each directory is a label (recipient here)
and instances are placed in the directories. File names are unique message id
used in enron database. Only consider 'TO' list.

Due to a lot of operations related to file system, the code runs quite slowly
(it took several minutes in my laptop). I guess directory operations are really
slow. In the future, we might have to figure out a way to directly write down a
feature file rather than making directory structrue and importing them into a
feature file.

The code assumes that the file 'top5people-sentbox.json' exits in the same
directory. The dataset size amounts to approximately half of the entire dataset.

Currently, the directory structure does not reflect the date. However, the file
'top5people-sentbox.json' also includes the date information and I can modify
this code later to generate time-sensitive training set and test set.
"""
os.mkdir(root)

for sender, messages in doc.iteritems():
    # for each sender, we make a separate dataset
    sname = re.match(r'(.*)@.*', sender).group(1).strip()
    datadir = ''.join([root, sname, '/'])
    os.mkdir(datadir)

    for m in messages:
        if m['rtype'] == 'TO':
            # create a label directory is not exists
            rname = re.match(r'(.*)@.*', m['recipient']).group(1).strip()
            labeldir = ''.join([datadir, rname, '/'])
            if not os.path.exists(labeldir):
                os.mkdir(labeldir)

            filename = ''.join([labeldir, str(m['mid'])])
            with open(filename, 'w') as f:
                f.write(m['body'])
