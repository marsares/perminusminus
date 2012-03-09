
cxx=g++ -O3 -I .. -g
all: train predict
train: train.cc *.h
	$(cxx) train.cc -o train
predict: predict.cc *.h
	$(cxx) predict.cc -o predict
