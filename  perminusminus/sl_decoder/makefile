

sl_decoder: sl_decoder.cpp ../path_labeling/decoder.h
	g++ -c sl_decoder.cpp -o sl_decoder.o -O3 -I ../path_labeling
	g++ -c tagging_decoder.cpp -o tagging_decoder.o -O3 -I ../path_labeling
	g++ -o sl_decoder sl_decoder.o tagging_decoder.o
