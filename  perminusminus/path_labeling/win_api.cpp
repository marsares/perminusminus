/* FILE statederivative.c */
#include "dll.h"
#include<stdio.h>
#include<stdlib.h>
#include"scripts.h"

#ifdef __cplusplus
extern "C" {
#endif

EXPORT void permm_test(char* modelfile,char*testfile,char*resultfile,int decode_type){
    test(modelfile,testfile,resultfile,decode_type);
};

EXPORT void permm_train(char* trainingfile,char* modelfile,int iterations){
    train(trainingfile,modelfile,iterations);
};

#ifdef __cplusplus
}
#endif
