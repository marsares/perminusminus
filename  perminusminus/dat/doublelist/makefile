double_list: double_list.cpp
	g++ -o double_list -O3 double_list.cpp
dmaker: dat_builder.cpp lex2.txt makefile
	g++ -g -o dmaker dat_builder.cpp -O3
	./dmaker dat.bin < lex2.txt
test:
	g++ -g -o dat dat.cpp -O3
	./dat dat.bin
