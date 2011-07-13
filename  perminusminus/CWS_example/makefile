evaluation: result.txt
	/usr/bin/python3 ./eval.py result.txt
result.txt : model.bin test.bin per--
	./per-- p model.bin test.bin result.txt 0
training.bin : msr_training.utf8 cws_transform.py
	/usr/bin/python3 ./cws_transform.py l "msr_training.utf8" training.bin index.json
test.bin : index.json msr_test_gold.utf8 cws_transform.py
	/usr/bin/python3 ./cws_transform.py p index.json msr_test_gold.utf8 test.bin
model.bin : training.bin per--
	./per-- l training.bin model.bin 25

