#!/bin/bash

../path_labeling/per-- l "data/training.bin" "data/model.bin" 15
../path_labeling/per-- p "data/model.bin" "data/test.bin" "data/result.txt" 0


#~/exp/wheels/dat/c_dat.py data/index.txt data/index.bin
#../sl_decoder/sl_decoder data/model.bin data/index.bin data/label_info.txt <data/ctb_test_raw.txt > data/ctb_test_result.txt

