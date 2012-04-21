include Makefile.conf

FLAGS=-lboost_serialization -lgrante -O0 -g
OUT=-o a.out
ARGS=$(INCLUDE) $(LIBRARY) $(FLAGS) $(OUT)

all:
	$(CC) main.cpp svmlightReader.cpp factorBuilder.cpp $(ARGS)

naivetest:
	$(CC) $(ARGS) naivebayes_test.cpp naivebayes.cpp

sample:
	$(CC) $(ARGS) sample.cpp


