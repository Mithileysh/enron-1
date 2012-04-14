<link href="http://kevinburke.bitbucket.org/markdowncss/markdown.css"
rel="stylesheet"></link>

## Quick Start

### Downloaing scripts from this Git repository

First clone this git repository by typing in your working directory:

    git clone git://github.com/pyongjoo/enron.git

It will create a directory *enron*. Otherwise, if you want to clone the dataset
in your current working directory, type:

    git clone git://github.com/pyongjoo/enron.git .
    

### Generating Dataset

(Skip this part if you don't wanna regenerate the file from the database. This git repository is already including the file.). After download has finished, type the following to dump mysql data into a json file:

    ./dumpIntoFile.py | python -mjson.tool > top5people-sentbox.json
    
After that, we can dump again the file contents into labeled directory by entering:

    ./dumpFileIntoDir.py [output directory]

or

    python dumpFileIntoDir.py [output directory]

It takes several minutes to finish (less than a minute in desktop). The code generates following directory structure:

- [output directory]
    - sender 1
        - receiver 1
            + file 1
            + file 2
            + f...
        - receiver 2
        - r...
    - sender 2
    - s...

Fow now, the code does not generate training set and test set separately
according to the written date. It should be done in the near future.


### Running Naive Bayes classifier using Mallet library

*importdir.sh* - Import the created dataset into a mallet format (only for Naive Bayes):

    #!/bin/bash
    MalletHome=/home/yongjoo/workspace/mallet-2.0.7
    InputDir=exclusiveDataset/jeff.dasovich/*
    OutputFile=exclusiveEnron.mallet
    CMD="$MalletHome/bin/mallet import-dir \
    --input $InputDir \
    --output $OutputFile"
    $CMD


*runMallet.sh* - Run Naive Bayes trainer with 10-fold cross validation:

    #!/bin/bash
    MalletHome=/home/yongjoo/workspace/mallet-2.0.7
    InputFile=exclusiveEnron.mallet
    CMD="$MalletHome/bin/mallet train-classifier \
    --input $InputFile \
    --cross-validation 10"
    $CMD


### Running LDA for feature space reduction

*importdirTopicModel.sh* - Import the created dataset into a mallet format (only for Topic modelling):

    #!/bin/bash
    MalletHome=/home/yongjoo/workspace/mallet-2.0.7
    InputDir=exclusiveDataset/jeff.dasovich/*
    OutputFile=topic-input.mallet
    CMD="$MalletHome/bin/mallet import-dir \
      --input $InputDir \
      --output $OutputFile \
      --keep-sequence \
      --remove-stopwords"
    #echo $CMD
    $CMD

*runTopicMallet.sh* - Run Topic Modeling Mallet library and save topics for each doc into a file:

    #!/bin/bash
    MalletHome=/home/yongjoo/workspace/mallet-2.0.7
    InputFile=topic-input.mallet
    OutputFile=doc-topics.txt
    CMD="$MalletHome/bin/mallet train-topics \
      --input $InputFile \
      --num-topics 100 \
      --output-doc-topics $OutputFile"
    $CMD

### Running Naive Bayes with topics extracted from LDA

Converting mallet topic document to SVM-lignt format:

    ./malletTopicToSVMLight.py doc-topics.txt doc-topic-svmlight.txt

*importsvmlight.sh* - Import SVM-light format into mallet format:

    #!/bin/bash
    MalletHome=/home/yongjoo/workspace/mallet-2.0.7
    InputFile=doc-topic-svmlight.txt
    OutputFile=enronInTopics.mallet
    CMD="$MalletHome/bin/mallet import-svmlight \
        --input $InputFile \
        --output $OutputFile"
    $CMD
    
*runMalletForTopic.sh* - Now run Naive Bayes classifier with the data obtained from the above step:

    #!/bin/bash
    MalletHome=/home/yongjoo/workspace/mallet-2.0.7
    InputFile=enronInTopics.mallet
    CMD="$MalletHome/bin/mallet train-classifier \
    --input $InputFile \
    --cross-validation 10"
    $CMD



## The list of top users and Date

    mysql> select sender, count(*), min(date), max(date)
    from message
    group by sender
    order by count(*) desc limit 5;
    +---------------------------+----------+---------------------+---------------------+
    | sender                    | count(*) | min(date)           | max(date)           |
    +---------------------------+----------+---------------------+---------------------+
    | jeff.dasovich@enron.com   |     6272 | 1999-12-03 10:27:00 | 2002-09-22 09:42:25 |
    | kay.mann@enron.com        |     5100 | 2000-06-02 06:10:00 | 2002-05-28 15:02:28 |
    | sara.shackleton@enron.com |     4797 | 1999-05-10 01:57:00 | 2002-03-25 13:49:25 |
    | tana.jones@enron.com      |     4437 | 1999-05-03 03:37:00 | 2002-02-08 14:13:53 |
    | vince.kaminski@enron.com  |     4366 | 1999-12-10 05:12:00 | 2001-06-20 09:40:00 |
    +---------------------------+----------+---------------------+---------------------+
    5 rows in set (0.60 sec)


## MySQL Queries

### Email Data Extraction

Number of emails sent from a specific person:

    select m.mid as mid, m.sender as sender, r.rvalue as recipient,
    count(*) as count
    from message m
    inner join recipientinfo r
    on m.mid = r.mid
    where m.sender = "jeff.dasovich@enron.com" and
    r.rtype = 'TO'
    group by r.rvalue
    order by count asc;


To avoid the long tail from the above query:

    select * from (
        select m.mid as mid, m.sender as sender, r.rvalue as recipient,
        count(*) as sentcount
        from message m
        inner join recipientinfo r
        on m.mid = r.mid
        where m.sender = "jeff.dasovich@enron.com" and
        r.rtype = 'TO'
        group by r.rvalue
        order by sentcount asc) as T
    where T.sentcount > 10;


Retrieve emails sent from a specific user to users to whom the user sent emails more than 10 times:

    SELECT m.mid as mid, m.sender as sender, m.date as date,
    m.subject as subject, m.body as body, r.rtype as type,
    r.rvalue as recipient
    FROM message m
    inner join recipientinfo r
    on m.mid = r.mid
    WHERE m.sender = 'jeff.dasovich@enron.com' and
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
            order by sentcount asc) as T
        where T.sentcount > 10
    )


Filter emails with only one recipient:

    select mid
    from (
        SELECT m.mid as mid, r.rvalue as recipient, count(*) as count
        FROM message m
        inner join recipientinfo r
        on m.mid = r.mid
        where m.sender in
            (select * from topSenders) and
        r.rtype = 'TO'
        GROUP by mid) as T;
    WHERE T.count = 1;


Retrieve emails sent from a specific user to users to whom the user sent emails more than 10 times, and with only one recipient:

    CREATE table topSenders
    SELECT sender
    FROM message
    GROUP BY sender
    ORDER BY count(*) desc limit 5

    SELECT m.mid as mid, m.sender as sender, m.date as date,
    m.subject as subject, m.body as body, r.rtype as type,
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
    )


### Time Dynamics

See the number of emails sent grouped by recipient and year/month:

    SELECT r.rvalue as recipient, year(m.date) as year,
    month(m.date) as month, count(*) as count
    
    FROM message m
    inner join recipientinfo r
    on m.mid = r.mid
    
    WHERE m.sender = "jeff.dasovich@enron.com" and
    r.rtype = 'TO' and
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
    )
    
    GROUP BY recipient, year(date), month(date)
    ORDER BY recipient, m.date asc;


