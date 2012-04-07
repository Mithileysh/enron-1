#!/usr/bin/python

import re, sys

if len(sys.argv) < 2:
  print "USAGE: ./mysqlcompatibility.py [outfile]"

filename = 'another-enron-mysqldump.sql'
outfile = open(sys.argv[1], 'w')

for line in open(filename):
  line = re.sub('TYPE=', 'ENGINE=', line)
  outfile.write(line)
