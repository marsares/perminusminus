

sl_decoder: sl_decoder.cpp ../path_labeling/decoder.h
	g++ -c sl_decoder.cpp -o sl_decoder.o -O3 -I ../path_labeling
	g++ -c tagging_decoder.cpp -o tagging_decoder.o -O3 -I ../path_labeling
	g++ -o sl_decoder sl_decoder.o tagging_decoder.o


dagger:
	g++ -c dagger.cpp -o dagger.o -O3 -I ../path_labeling
	g++ -c tagging_decoder.cpp -o tagging_decoder.o -O3 -I ../path_labeling
	g++ -o dagger dagger.o tagging_decoder.o
	


dagger_test: dagger
	valgrind --tool=memcheck --leak-check=full  ./dagger ctb5/seg.model ctb5/seg.dat ctb5/seg.label_index  ctb5/tag.model ctb5/tag.dat ctb5/tag.label_index < test.in.txt > test.out.txt
	#time echo "材料利用率高" | ./dagger.sh
	#time cat msr_test.utf8 | ./dagger.sh -t 15 > test_out.txt

dagger_clear:
	rm dagger

cws_test:
	g++ -g -c sl_decoder.cpp -o sl_decoder.o -O3  -I ../path_labeling
	g++ -g -c tagging_decoder.cpp -o tagging_decoder.o -O3 -I ../path_labeling
	g++ -g -o sl_decoder sl_decoder.o tagging_decoder.o
	valgrind --track-origins=yes --num-callers=10 --tool=memcheck --leak-check=full  ./sl_decoder models/cws_model.bin models/cws_dat.bin models/cws_label_info.txt < test.in.txt > test.out.txt
lib:
	g++ -g -c lib_decoder.cpp -o lib_decoder.o -O3  -fPIC -I ../path_labeling
	g++ -g -c tagging_decoder.cpp -o tagging_decoder.o -O3 -fPIC -I ../path_labeling
	g++ -g -fPIC -shared lib_decoder.o tagging_decoder.o -o libcws.so
	valgrind --track-origins=yes --num-callers=10 --tool=memcheck --leak-check=full ./testcwslib.py
