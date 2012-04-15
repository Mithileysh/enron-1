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

    # We will write a fg file as we read the data
    fg = open('recipients.fg', 'w')
    fg.write(str(len(rows) - 1) + '\n')  # number of factors

    # this is to count the number of unique recipients
    for row in rows:
        recipientIndexer.addName(row['recipient'])

    # not print the labeled contents to file
    for i, row in enumerate(rows[0:len(rows)-1]):
        recipient = row['recipient']
        body = row['body']

        fg.write('\n')          # put blank line before each factor block
        fg.write('2\n')
        fg.write(str(i) + ' ' + str(i+1) + '\n')
        dim = recipientIndexer.size()
        fg.write(str(dim) + ' ' + str(dim) + '\n')
        fg.write(str(dim * dim) + '\n')     # number of entries

        for k in range(dim * dim):
            fg.write(str(k) + '\t' + str(float(1) / float(dim * dim)) + '\n')

    fg.close()


    # We will write a tab file
    tab = open('recipients.tab', 'w')

    # print header
    for i in range(len(rows)):
        tab.write(str(i))
        if i != len(rows) - 1:
            tab.write('\t')

    tab.write('\n\n')

    # print evidence
    for i, row in enumerate(rows):
        label = recipientIndexer.getIndex(row['recipient'])
        tab.write(str(label))
        if i != len(rows) - 1:
            tab.write('\t')

    tab.write('\n')
    tab.close()


    # We will write a parameter estimation file
    em = open('recipients.em', 'w')

    # print header
    em.write('1\n\n')

    # print maximization step
    em.write('1\n')     # number of shared parameters block
    em.write('RecipientProbEstimation [target_dim=' +
            str(dim) + ',total_dim=' +
            str(dim * dim) + ']\n')

    em.write(str(len(rows) - 1) + '\n')    # number of factors 

    for i in range(len(rows) - 1):
        em.write(str(i) + ' ' +
                str(i) + ' ' +
                str(i + 1) + '\n')

    em.close()


except mdb.Error, e:
    print "Error %d: %s" % (e.args[0],e.args[1])
    sys.exit(1)

finally:
    if con:
        con.close()

