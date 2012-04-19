CC=g++
INCLUDE=-I/usr/include -I/home/yongjoo/workspace/grante-1.0/src
LIBRARY=-L/usr/lib -L/home/yongjoo/workspace/grante-1.0/lib
FLAGS=-lboost_serialization -lgrante -O2
OUT=-o a.out
ARGS=$(FLAGS) $(INCLUDE) $(LIBRARY) $(OUT)

all:
	$(CC) main.cpp svmlightReader.cpp factorBuilder.cpp $(ARGS)

naivetest:
	$(CC) $(ARGS) naivebayes_test.cpp naivebayes.cpp

sample:
	$(CC) $(ARGS) sample.cpp


