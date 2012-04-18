CC=g++47
INCLUDE=-I/opt/local/include -I/Users/yongjoo/workspace/grante-1.0/src
LIBRARY=-L/opt/local/lib -L/Users/yongjoo/workspace/grante-1.0/lib/fast
FLAGS=-lboost_serialization -lgrante -O2
OUT=-o a.out
ARGS=$(INCLUDE) $(LIBRARY) $(FLAGS) $(OUT)

all:
	$(CC) $(ARGS) main.cpp svmlightReader.cpp factorBuilder.cpp

naivetest:
	$(CC) $(ARGS) naivebayes_test.cpp naivebayes.cpp

sample:
	$(CC) $(ARGS) sample.cpp


