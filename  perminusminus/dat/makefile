all: clear dat_builder dat


dat_builder: dat.h dat_builder.cc ../base/daidai_base.h
	g++ -g -o dat_builder dat_builder.cc -O3

dat: dat.cc dat.h ../base/daidai_base.h
	g++ -g -o dat dat.cc -O3

test: dat_builder dat
	./dat_builder dat.bin -f ./doublelist/lex2.txt
	./dat dat.bin

clear:
	rm dat_builder -f
	rm dat -f

old: dat_builder
	./dat_builder dat.bin -s -P -l ./doublelist/lex2.txt
