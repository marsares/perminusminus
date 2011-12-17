#sl_decoder: sl_decoder.o tagging_decoder.o
#	g++ -o sl_decoder sl_decoder.o tagging_decoder.o

#sl_decoder.o: sl_decoder.cpp tagging_decoder.h tagging_decoder.o ../path_labeling/decoder.h
#	g++ -c sl_decoder.cpp -o sl_decoder.o -O3 -I ../path_labeling


sl_decoder: sl_decoder.cpp ../path_labeling/decoder.h
	g++ sl_decoder.cpp -o sl_decoder -O3 -I ../path_labeling

#tagging_decoder.o: tagging_decoder.h tagging_decoder.cpp ../path_labeling/decoder.h
#	g++ -c tagging_decoder.cpp -o tagging_decoder.o -O3 -I ../path_labeling

