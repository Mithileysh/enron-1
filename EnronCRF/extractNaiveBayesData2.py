#!/usr/bin/python

import MySQLdb as mdb
import sys
import re


query = """
SELECT m.mid as mid, m.sender as sender, m.date as date,
m.subject as subject, r.rtype as type, m.body as body,
r.rvalue as recipient
FROM message m
inner join recipientinfo r
on m.mid = r.mid
WHERE m.sender = 'jeff.dasovich@enron.com' and
m.mid in (
    select mid
    from (
        SELECT m.mid as mid, r.rvalue as recipient, count(*) as count
        FROM message m
        inner join recipientinfo r
        on m.mid = r.mid
        where m.sender in
            (select * from topSenders) and
        r.rtype = 'TO'
        GROUP by mid) as T
    WHERE T.count = 1
) and
r.rvalue in (
    select recipient from (
        select m.mid as mid, m.sender as sender, r.rvalue as recipient,
        count(*) as sentcount
        from message m
        inner join recipientinfo r
        on m.mid = r.mid
        where m.sender = 'jeff.dasovich@enron.com' and
        r.rtype = 'TO'
        group by r.rvalue
        order by sentcount asc) as S
    where S.sentcount > 10
) and
r.rtype = 'TO'
order by m.date
limit 1, 3
"""

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

recipientIndexer = RecipientIndexer()


con = None

try:
    con = mdb.connect('localhost', 'root', 'Fgla4Zp0', 'enron');

    cur = con.cursor(mdb.cursors.DictCursor)
    cur.execute(query)

    rows = cur.fetchall()

    fg = open('naive.fg', 'w')
    tab = open('naive.tab', 'w')
    em = open('naive.em', 'w')

    numFactors = 0
    for row in rows:
        recipient = row['recipient']
        body = row['body']

        recipientIndexer.addName(recipient)

        words = re.findall(r'\w+', body)
        numFactors += len(words)

        for word in words:
            wordIndexer.getIndex(word)

    fg.write(str(numFactors) + '\n')

    tabHeader = ""
    tabEvidence = ""

    # label index
    parentLabelIndex = 0
    childLabelIndex = 0
    nextLabelIndex = 0

    # factor index
    currentFactor = 0
    nextFactor = 0

    # write em file
    em.write('1\n\n')   # number of maximization step
    em.write('1\n')     # number of shared blocks
    em.write('CondProbEstimation [target_dim=' +
            str(recipientIndexer.size()) + ',total_dim=' +
            str(recipientIndexer.size() * wordIndexer.size()) + ']\n')
    em.write(str(numFactors) + '\n')

    for row in rows:
        recipient = row['recipient']
        body = row['body']
        words = re.findall(r'\w+', body)

        parentLabelIndex = nextLabelIndex
        nextLabelIndex += 1

        tabHeader += str(parentLabelIndex) + '\t'
        tabEvidence += str(recipientIndexer.getIndex(recipient)) + '\t'

        for word in words:
            childLabelIndex = nextLabelIndex
            nextLabelIndex += 1

            tabHeader += str(childLabelIndex) + '\t'
            tabEvidence += str(wordIndexer.getIndex(word)) + '\t'

            fg.write('\n' + str(2) + '\n')
            fg.write(str(parentLabelIndex) + ' ' + str(childLabelIndex) + '\n')
            fg.write(str(recipientIndexer.size()) + ' '
                    + str(wordIndexer.size()) + '\n')

            fg.write('1\n')
            fg.write('0 1\n')

            #fg.write(str(recipientIndexer.size() * wordIndexer.size()) + '\n')

            #for i in range(recipientIndexer.size() * wordIndexer.size()):
                #fg.write(str(i) + ' 1\n')

            currentFactor = nextFactor
            nextFactor += 1

            em.write(str(currentFactor) + ' '
                    + str(parentLabelIndex) + ' '
                    + str(childLabelIndex) + '\n')

    tab.write(tabHeader.rstrip('\t') + '\n\n')
    tab.write(tabEvidence.rstrip('\t') + '\n')

except mdb.Error, e:
    print "Error %d: %s" % (e.args[0],e.args[1])
    sys.exit(1)

finally:
    if con:
        con.close()

