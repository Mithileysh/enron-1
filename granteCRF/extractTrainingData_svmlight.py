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
limit 500
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

# stop list process
stoplist = []
for line in open('../stoplist.txt'):
    word = line.rstrip('\n')
    stoplist.append(word)

def isStopWord(word):
    return True if word in stoplist else False


con = None

try:
    con = mdb.connect('localhost', 'root', 'Fgla4Zp0', 'enron');

    cur = con.cursor(mdb.cursors.DictCursor)
    cur.execute(query)

    rows = cur.fetchall()

    of = open('midtraining.svmlight', 'w')

    for row in rows:
        r = row['recipient']
        #words = re.findall(r'\w+', row['body'] + row['subject'])
        words = re.findall(r'\w+', row['body'])

        of.write(str(recipientIndexer.getIndex(r)))

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


        of.write(featureLine + '\n')


except mdb.Error, e:
    print "Error %d: %s" % (e.args[0],e.args[1])
    sys.exit(1)

finally:
    if con:
        con.close()

