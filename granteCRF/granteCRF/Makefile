include Makefile.conf

FLAGS=-lboost_serialization -lgrante -O2
OUT=-o a.out
ARGS=$(INCLUDE) $(LIBRARY) $(FLAGS) $(OUT)

all:
	$(CC) main.cpp svmlightReader.cpp factorBuilder.cpp $(ARGS)

naivetest:
	$(CC) $(ARGS) naivebayes_test.cpp naivebayes.cpp

sample:
	$(CC) $(ARGS) sample.cpp

run:
	./a.out /home/yongjoo/workspace/enron/dataset/train_out /home/yongjoo/workspace/enron/dataset/test_out /home/yongjoo/workspace/enron/dataset/test_observed_label

