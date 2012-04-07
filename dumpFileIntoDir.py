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
This code reads the dataset from the file and dump the contents into directories
in a directory given as an argument. Each directory is a label (recipient here)
and instances are placed in the directories. File names are unique message id
used in enron database
"""
os.mkdir(root)

for sender, messages in doc.iteritems():
    for m in messages:
        if m['rtype'] == 'TO':
            # create a label directory is not exists
            name = re.match(r'(.*)@.*', sender).group(1)
            labeldir = root.join([name, '/'])
            if not os.path.exists(labeldir):
                os.mkdir(labeldir)

            filename = labeldir.join([str(m['mid'])])
            with open(filename, 'w') as f:
                f.write(m['body'])
