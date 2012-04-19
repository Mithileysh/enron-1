include Makefile.conf

FLAGS=-lboost_serialization -lgrante -O2
OUT=-o a.out
ARGS=$(INCLUDE) $(LIBRARY) $(FLAGS) $(OUT)

all:
	$(CC) $(ARGS) main.cpp svmlightReader.cpp factorBuilder.cpp

naivetest:
	$(CC) $(ARGS) naivebayes_test.cpp naivebayes.cpp

sample:
	$(CC) $(ARGS) sample.cpp


