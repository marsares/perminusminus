all: dagger train predict


OptArg=-O3
#OptArg=

project_root=..

cxx=g++ $(OptArg) -I ..

tagging_learner.o: tagging_learner.cc tagging_learner.h tagging_decoder.h ngram_feature.h model.h $(project_root)/base/* decoder.h dat.h
	$(cxx) -g -c tagging_learner.cc -o tagging_learner.o

tagging_decoder.o: tagging_decoder.cc tagging_decoder.h ngram_feature.h ngram_feature.h model.h decoder.h $(project_root)/base/* dat.h
	$(cxx) -g -c tagging_decoder.cc -o tagging_decoder.o


sl_decoder: tagging_decoder.h sl_decoder.cc $(project_root)/* $(project_root)/base/*
	$(cxx) sl_decoder.cc -o sl_decoder


train: tagging_learner.h train.cc tagging_decoder.h ngram_feature.h
	$(cxx) -g -o train train.cc

predict: predict.cc tagging_decoder.h
	$(cxx) -g -o predict predict.cc


dagger: tagging_decoder.h dagger.cc
	$(cxx) dagger.cc -o dagger
	


dagger_test: dagger
	valgrind --tool=memcheck --leak-check=full  ./dagger ctb5/seg.model ctb5/seg.dat ctb5/seg.label_index  ctb5/tag.model ctb5/tag.dat ctb5/tag.label_index < test.in.txt > test.out.txt
	#time echo "材料利用率高" | ./dagger.sh
	#time cat msr_test.utf8 | ./dagger.sh -t 15 > test_out.txt

dagger_clear:
	rm dagger

cws_test:
	g++ -g -c sl_decoder.cc -o sl_decoder.o -O3  -I ../path_labeling
	g++ -g -c tagging_decoder.cc -o tagging_decoder.o -O3 -I ../path_labeling
	g++ -g -o sl_decoder sl_decoder.o tagging_decoder.o
	valgrind --track-origins=yes --num-callers=10 --tool=memcheck --leak-check=full  ./sl_decoder models/cws_model.bin models/cws_dat.bin models/cws_label_info.txt < test.in.txt > test.out.txt
lib:
	g++ -g -c lib_decoder.cc -o lib_decoder.o -O3  -fPIC -I ../path_labeling
	g++ -g -c tagging_decoder.cc -o tagging_decoder.o -O3 -fPIC -I ../path_labeling
	g++ -g -fPIC -shared lib_decoder.o tagging_decoder.o -o libcws.so
	valgrind --track-origins=yes --num-callers=10 --tool=memcheck --leak-check=full ./testcwslib.py

libcws.so: tagging_decoder.h tagging_decoder.cc lib_decoder.h lib_decoder.cc dat.h
	g++ -g -c lib_decoder.cc -o lib_decoder.o -O3  -fPIC
	g++ -g -c tagging_decoder.cc -o tagging_decoder.o -O3 -fPIC
	g++ -g -fPIC -shared lib_decoder.o tagging_decoder.o -o libcws.so

poc_sequencer: tagging_decoder.h poc_sequencer.* dat.h
	$(cxx) -g poc_sequencer.cc -o poc_sequencer
	
poc: poc_sequencer
	echo '背景？我没去过背景啊！' | ./poc_sequencer

test_time: sl_decoder
	date >> time.log
	echo 'test time for cws.sh' >> time.log
	time -f "%E" ./cws.sh < msr_training.raw > output.txt 2>> time.log
	tail time.log
	diff output.txt stdout.txt | wc


clear:
	rm *.o -f
	rm train -f
	rm predict -f
	rm dagger -f
