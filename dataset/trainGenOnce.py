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


# stop list process
stoplist = []
for line in open('../stoplist.txt'):
    word = line.rstrip('\n')
    stoplist.append(word)

def isStopWord(word):
    return True if word in stoplist else False



with con:

    wordIndexer = WordIndexer()
    recipientIndexer = RecipientIndexer()

    train_table = 'data_train'
    test_table = 'data_test'

    cur = con.cursor(mdb.cursors.DictCursor)

    sender = "richard.shapiro@enron.com"
    fileprefix = "shapiro"

    param_query = """
    select mid, sender, rvalue, rtype, date, subject, body
    from
    (select m.mid, m.sender, m.date, m.subject,
    m.body, r.rvalue, r.rtype

    from message m
    inner join recipientinfo r
    on m.mid = r.mid
    inner join %(table)s d
    on m.mid = d.mid

    where (m.sender = '%(sender)s' and
    m.mid in (
        select mid
        from (
            SELECT m.mid as mid, r.rvalue as recipient, count(*) as count
            FROM message m
            inner join recipientinfo r
            on m.mid = r.mid

            where m.mid in
                (select * from %(table)s) and
            r.rtype = 'TO'

            GROUP by mid) as T
        WHERE T.count = 1) and
    r.rtype = 'TO'
    ) or

    r.rvalue = '%(sender)s'

    order by m.date) as T;
    """

    query = param_query % dict(sender=sender, table=train_table)

    cur.execute(query)

    # store instances in svm-light format
    train_out = open(fileprefix + '.train', 'w')

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


    # Now write a file to ( I don't remember what comment meant)

    query = param_query % dict(sender=sender, table=test_table)

    cur.execute(query)

    test_out = open(fileprefix + '.test', 'w')

    # test set is partially observed
    observed_out = open(fileprefix + '.test_observed_label', 'w')
    hidden_out = open(fileprefix + '.test_hidden_label', 'w')

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
