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


EXPORT void permm_init(char* modelfile,int decode_type,void*& pointer){
    Model* model=new Model(modelfile);
    //printf("model loaded\n");
    pointer=new PERMM(model,(decode_type>0?decode_type:1));
};
EXPORT void permm_debug(void*&permm){
    //printf("%d\n",((PERMM*)permm)->length);
    //printf("%d\n",permm_permm->length);
    //printf("%d\n",debug_x);
};


EXPORT void permm_parse_graph(char* buffer,void* &graph){
    parse_graph((int*)buffer,(Graph*&)graph);
};

EXPORT void permm_decode(void*&permm,void*&graph,int*&result){
    ((PERMM*)permm)->decode((Graph*&)graph);
    result=((PERMM*)permm)->result;
};

EXPORT void permm_decode_2(void*&permm,void*&graph,int*&result,int*&values,int*&alphas,int*&betas){
    ((PERMM*)permm)->decode((Graph*&)graph);
    ((PERMM*)permm)->cal_betas((Graph*&)graph);
    result=((PERMM*)permm)->result;
    values=(int*)((PERMM*)permm)->values;
    alphas=(int*)((PERMM*)permm)->alphas;
    betas=(int*)((PERMM*)permm)->betas;
};

#ifdef __cplusplus
}
#endif
