#!/bin/bash

##transform
#./transform.py -l -s \_  -d slex.txt data/training.seg tmp/training.bin tmp/index.txt tmp/label_index.txt
#./transform.py -p -s \_  -d slex.txt data/test.seg tmp/test.bin tmp/index.txt tmp/label_index.txt

#./transform.py -l -s \   data/training.seg tmp/training.bin tmp/index.txt tmp/label_index.txt
#./transform.py -p -s \  data/test.seg tmp/test.bin tmp/index.txt tmp/label_index.txt
            


./transform.py -l -d slex.txt data/training.tag tmp/training.bin tmp/index.txt tmp/label_index.txt
./transform.py -p -d slex.txt data/test.tag tmp/test.bin tmp/index.txt tmp/label_index.txt

../path_labeling/per-- l "tmp/training.bin" "tmp/model.bin" 15
../path_labeling/per-- p "tmp/model.bin" "tmp/test.bin" "tmp/result.txt" -1

#evaluation
./tagging_eval.py

#~/exp/wheels/dat/c_dat.py data/index.txt data/index.bin
#../sl_decoder/sl_decoder data/model.bin data/index.bin data/label_info.txt <data/ctb_test_raw.txt > data/ctb_test_result.txt

