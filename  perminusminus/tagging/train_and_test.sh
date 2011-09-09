#!/bin/bash
../path_labeling/per-- l "data/training.bin" "data/model.bin" 10
../path_labeling/per-- p "data/model.bin" "data/test.bin" "data/result.txt" -2
