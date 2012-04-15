#!/usr/bin/python

import os
import re
import sys

datadir = '/Users/yongjoo/workspace/enron/exclusiveDataset/jeff.dasovich'
labels = os.listdir(datadir)

# Labels are domains of the parent node
# A parent node is linked with many feature nodes, and each child node
# represents an existence of the word.
# The evidence line will be an really sparse vector

class WordIndexer:
    def __init__(self):
        self.wordDic = {}

    def getIndex(self, word):
        if word in self.wordDic:
            return self.wordDic[word]
        else:
            self.wordDic[word] = len(self.wordDic)
            return self.wordDic[word]

    def size(self):
        return len(self.wordDic)


wordIndexer = WordIndexer()


# write tab file

tab = open('naive.tab', 'w')
tab.write('0\t1\n\n')

for index, label in enumerate(labels):
    path_to_label = os.path.join(datadir, label)
    files = os.listdir(path_to_label)

    for fname in files:
        f = os.path.join(path_to_label, fname)
        words = re.findall(r'\w+', open(f).read())

        for w in words:
            tab.write(str(index) + '\t' +
                    str(wordIndexer.getIndex(w)) + '\n')

tab.close()


# write fg file

dim = len(labels) * wordIndexer.size()

fg = open('naive.fg', 'w')
fg.write('1\n\n')   # number of factors

fg.write('2\n')     # number of involved variables
fg.write('0 1\n')   # involved labels
fg.write(str(len(labels)) + ' '
        + str(wordIndexer.size()) + '\n')  # dimension of variables

fg.write(str(dim) + '\n')     # number of non-zero entry in CPT (meaningless)

for i in range(dim):
	fg.write(str(i) + '\t' + str(1 / float(dim)) + '\n')

fg.close()


# write em file

em = open('naive.em', 'w')
em.write('1\n\n')   # number of shared parameter blocks

em.write('1\n')     # block label
em.write('CondProbEstimation '
        + '[target_dim=' + str(len(labels))
        + ',total_dim=' + str(dim) + ']\n') # estimator
em.write('1\n')     # number of parameters to estimate
em.write('0 0 1\n')   # target to estimate

em.close()

