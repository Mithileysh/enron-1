#!/usr/bin/python
# -*- coding: utf-8 -*-

import MySQLdb as mdb
import sys, json

def dumpIntoDir():
    # All the retrieved data are stored in this hash table structure
    # { 'sender 1': [mail 1, mail 2, ...]
    #   'sender 2': ...
    #   ...
    # }
    #
    # And the mail object is as follows:
    # mail =
    # { 'mid': ..
    #   'recipient': ..
    #   'rtype': ..
    #   'date': ..
    #   'subject': ..
    #   'body': ..
    # }
    doc = {}


    con = mdb.connect('localhost', 'root', 'Fgla4Zp0', 'enron')

    with con:

        cur = con.cursor(mdb.cursors.DictCursor)
        slist = ['jeff.dasovich@enron.com',
            'kay.mann@enron.com',
            'sara.shackleton@enron.com',
            'tana.jones@enron.com',
            'vince.kaminski@enron.com']

        for sender in slist:
            query = """
            SELECT m.mid as mid, m.sender as sender, m.date as date,
            m.subject as subject, m.body as body, r.rtype as type,
            r.rvalue as recipient
            FROM message m
            inner join recipientinfo r
            on m.mid = r.mid
            WHERE m.sender = '%s' and
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
                    where m.sender = '%s' and
                    r.rtype = 'TO'
                    group by r.rvalue
                    order by sentcount asc) as S
                where S.sentcount > 10
            )
            """ % (sender, sender)

            cur.execute(query)

            while True:
                row = cur.fetchone()
                if not row:
                    break

                # construct a mail object
                mail = {'mid': row['mid'],
                        'recipient': row['recipient'],
                        'rtype': row['type'],
                        'date': unicode(row['date'].replace(microsecond=0)),
                        'subject': row['subject'],
                        'body': row['body']}

                if not sender in doc:
                    doc[sender] = []

                doc[sender].append(mail)

    return doc


if __name__ == "__main__":
    print json.dumps(dumpIntoDir())
