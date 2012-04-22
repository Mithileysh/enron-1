#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb as mdb
import sys
import json
import re

con = mdb.connect('localhost', 'root', 'Fgla4Zp0', 'enron')

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

class RecipientIndexer:
    def __init__(self):
        self.rDic = {}

    def getIndex(self, name):
        if name in self.rDic:
            return self.rDic[name]
        else:
            self.rDic[name] = len(self.rDic)
            return self.rDic[name]

    def size(self):
        return len(self.rDic)

    def addName(self, name):
        return self.getIndex(name)

    def isIn(self, name):
        if name in self.rDic:
            return True
        else:
            return False

recipientIndexer = RecipientIndexer()


# stop list process
stoplist = []
for line in open('../stoplist.txt'):
    word = line.rstrip('\n')
    stoplist.append(word)

def isStopWord(word):
    return True if word in stoplist else False



with con:

    cur = con.cursor(mdb.cursors.DictCursor)

    sender = "richard.shapiro@enron.com"

    query = """
    select sender, subject, body, rvalue
    from
    (select m.mid, m.sender, m.date, m.subject,
    m.body, r.rvalue, r.rtype

    from message m
    inner join recipientinfo r
    on m.mid = r.mid
    inner join data_chunk1 d
    on m.mid = d.mid

    where m.sender = '%s' or

    rvalue = '%s'

    order by m.date) as T;
    """ % (sender, sender)

    cur.execute(query)

    # store instances in svm-light format
    train_out = open('train_out', 'w')

    while True:
        row = cur.fetchone()

        if not row:
            break

        r = row['rvalue'] if row['sender'] == sender else row['sender']
        words = re.findall(r'\w+', row['body'] + row['subject'])

        train_out.write(str(recipientIndexer.getIndex(r)))

        # collect the word count for this recipient

        wordDic = {}

        for word in words:
            if isStopWord(word):
                continue
            else:
                wi = wordIndexer.getIndex(word)
                if wi in wordDic:
                    wordDic[wi] += 1
                else:
                    wordDic[wi] = 1


        # Now print the collected word into svm light format

        featureLine = ""

        for w, k in wordDic.iteritems():
            featureLine += ' ' + str(w) + ':' + str(k)


        train_out.write(featureLine + '\n')


    # Now write a file to

    sender = "richard.shapiro@enron.com"

    query = """
    select sender, subject, body, rvalue
    from
    (select m.mid, m.sender, m.date, m.subject,
    m.body, r.rvalue, r.rtype

    from message m
    inner join recipientinfo r
    on m.mid = r.mid
    inner join data_chunk2 d
    on m.mid = d.mid

    where m.sender = '%s' or

    rvalue = '%s'

    order by m.date) as T;
    """ % (sender, sender)

    cur.execute(query)

    test_out = open('test_out', 'w')

    # test set is partially observed
    observed_out = open('test_observed_label', 'w')
    hidden_out = open('test_hidden_label', 'w')

    rownum = 0

    while True:
        row = cur.fetchone()

        if not row:
            break

        r = row['rvalue'] if row['sender'] == sender else row['sender']
        words = re.findall(r'\w+', row['body'] + row['subject'])

        if not recipientIndexer.isIn(r):
            continue

        test_out.write(str(recipientIndexer.getIndex(r)))

        if row['sender'] == sender:
            hidden_out.write(str(rownum) + '\n')
        else:
            observed_out.write(str(rownum) + '\n')


        # collect the word count for this recipient

        wordDic = {}

        for word in words:
            if isStopWord(word):
                continue
            else:
                wi = wordIndexer.getIndex(word)
                if wi in wordDic:
                    wordDic[wi] += 1
                else:
                    wordDic[wi] = 1

        # Now print the collected word into svm light format

        featureLine = ""

        for w, k in wordDic.iteritems():
            featureLine += ' ' + str(w) + ':' + str(k)


        test_out.write(featureLine + '\n')

        rownum += 1
