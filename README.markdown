<link href="http://kevinburke.bitbucket.org/markdowncss/markdown.css"
rel="stylesheet"></link>

## Quick Start

First clone this git repository by typing in your working directory:

    git clone git://github.com/pyongjoo/enron.git

It will create a directory *enron*. Otherwise, if you want to clone the dataset
in your current working directory, type:

    git clone git://github.com/pyongjoo/enron.git .

After download has finished, run the following code:

    ./dumpFileIntoDir.py [output directory]

or

    python dumpFileIntoDir.py [output directory]

It takes several minutes to finish. The code generates following directory
structure:

- [output directory]
    - sender 1
        - receiver 1
            + file 1
            + file 2
            + ...
        - receiver 2
    - sender 2
    - ...

Fow now, the code does not generate training set and test set separately
according to the written date. It should be done in the near future.


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
