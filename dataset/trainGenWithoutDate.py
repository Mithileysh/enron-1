#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb as mdb
import sys
import json
import re
import datetime

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

# days to parameter conversion using fibonnachi
def transDayToParam(days):
    if days < 1:
        return 'ypa'
    elif days < 2:
        return 'ypb'
    elif days < 4:
        return 'ybc'
    elif days < 7:
        return 'ypd'
    elif days < 12:
        return  'ype'
    elif days < 28:
        return 'ypf'
    elif days < 33:
        return 'ypg'
    elif days < 53:
        return 'yph'
    else:
        return False


# extract time index
def extractTimeIndex(con, rows, cur):
    row = rows[cur]

    curdate = row['date']

    itr = cur - 1

    arr = []

    while itr >= 0:
        refdate = rows[itr]['date']

        daydelta = (curdate - refdate).days

        deltaparam = transDayToParam(daydelta)

        if deltaparam:
            rarr = getRArray(con, row['mid'])

            for r in rarr:
                rparam = r + "/" + deltaparam

                arr.append(rparam)

        else:
            break

        itr -= 1

    return arr


def getRArray(con, mid):
    rquery = """
    select mid, rvalue
    from recipientinfo
    where mid = %d
    """ % mid

    rcur = con.cursor(mdb.cursors.DictCursor)

    rcur.execute(rquery)

    r = []

    rrows = rcur.fetchall()

    for row in rrows:
        r.append(row['rvalue'])

    return set(r)


with con:

    wordIndexer = WordIndexer()
    recipientIndexer = RecipientIndexer()

    train_table = 'dataset_mid'

    cur = con.cursor(mdb.cursors.DictCursor)

    sender = "richard.shapiro@enron.com"
    fileprefix = "yong"

    param_query = """
    select m.mid as mid, m.sender as sender,
    m.date as date, m.subject as subject, m.body as body

    from message m
    inner join %(table)s d
    on m.mid = d.mid

    where m.sender = '%(sender)s'

    order by m.date
    """

    query = param_query % dict(sender=sender, table=train_table)

    cur.execute(query)

    # store instances in svm-light format
    train_out = open(fileprefix + '.train', 'w')

    rows = cur.fetchall()

    for idx, row in enumerate(rows):

        # collect recipients of this message

        rarr = getRArray(con, row['mid'])


        # collect the word count for this recipient
        # word count will include time stamp

        words = re.findall(r'\w+', row['body'] + row['subject'])

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

        #timewords = extractTimeIndex(con, rows, idx)

        #for word in timewords:
        #    wi = wordIndexer.getIndex(word)
        #    if wi in wordDic:
        #        wordDic[wi] += 1
        #    else:
        #        wordDic[wi] = 1


        # Now print the collected word into svm light format

        featureLine = ""

        for idx, r in enumerate(rarr):
            if idx > 0:
                featureLine += ":"

            featureLine += str(recipientIndexer.getIndex(r))

        for w, k in wordDic.iteritems():
            featureLine += ' ' + str(w) + ':' + str(k)


        train_out.write(featureLine + '\n')
