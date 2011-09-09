#!/bin/bash
echo transform data
./cws_transform.py l "msr_training.utf8" "tmp/training.bin" "tmp/index.txt"
./cws_transform.py p "tmp/index.txt" "msr_test_gold.utf8" "tmp/test.bin"

echo learn and predict
../path_labeling/per-- l "tmp/training.bin" "tmp/model.bin" 25
../path_labeling/per-- p "tmp/model.bin" "tmp/test.bin" "tmp/result.txt" 0

echo evaluation
./eval.py "tmp/result.txt"
