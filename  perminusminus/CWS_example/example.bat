echo off
echo transform data
c:\python31\python.exe cws_transform.py l "msr_training.utf8" "training.bin" "index.json"
c:\python31\python.exe cws_transform.py p "index.json" "msr_test_gold.utf8" "test.bin"

echo learn and predict
per~~.exe l "training.bin" "model.bin" 20
per~~.exe p "model.bin" "test.bin" "result.txt" 0

echo evaluation
c:\python31\python.exe eval.py