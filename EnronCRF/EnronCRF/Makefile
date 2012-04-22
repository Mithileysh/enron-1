all:
	g++ -O3 -Wl,--no-as-needed -I/usr/include -L/usr/lib -lgmpxx -lgmp -ldai -o a.out *.cpp /usr/lib/libdai.a

run:
	./a.out ../naive.fg ../naive.tab ../naive.em
