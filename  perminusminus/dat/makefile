all: clear dat_builder dat


dat_builder: dat.h dat_builder.cpp ../base/daidai_base.h
	g++ -g -o dat_builder dat_builder.cpp -O3

dat: dat.cpp dat.h ../base/daidai_base.h
	g++ -g -o dat dat.cpp -O3

test: dat_builder dat
	./dat_builder dat.bin -f ./doublelist/lex2.txt
	./dat dat.bin

clear:
	rm dat_builder
	rm dat

old: dat_builder
	./dat_builder dat.bin -s -P -l ./doublelist/lex2.txt
