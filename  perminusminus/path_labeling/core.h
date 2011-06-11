#ifndef __CORE_H__
#define __CORE_H__
#include "dp.h"
#include<stdio.h>
#include "graph_file.h"
/*the structure of the model, also the file of the model*/
struct Model{
    int l_size;//size of the labels
    int f_size;//size of the features
    int* ll_weights;
    int* fl_weights;//weights of (feature,label)
    
    double* ave_ll_weights;
    double* ave_fl_weights;//weights of (feature,label)
    
    int is_learning;
    Model(int l,int f,int is_learning=0){
        this->l_size=l;
        this->f_size=f;
        this->ll_weights=(int*)calloc(sizeof(int),l*l);
        this->fl_weights=(int*)calloc(sizeof(int),l*f);
        if(is_learning){
            this->ave_ll_weights=(double*)calloc(sizeof(double),l*l);
            this->ave_fl_weights=(double*)calloc(sizeof(double),l*f);
        }
        this->is_learning=is_learning;
    }
    Model(char* filename){
        FILE* pFile;
        pFile=fopen(filename,"rb");
        fread(&(this->l_size),4,1,pFile);
        fread(&(this->f_size),4,1,pFile);
        int l_size=this->l_size;
        int f_size=this->f_size;
        printf("%d %d\n",this->l_size,this->f_size);
        this->ll_weights=(int*)malloc(sizeof(int)*l_size*l_size);
        this->fl_weights=(int*)malloc(sizeof(int)*l_size*f_size);
        
        fread((this->ll_weights),4,l_size*l_size,pFile);
        fread((this->fl_weights),4,l_size*f_size,pFile);
    }
    ~Model(){
        free(this->ll_weights);
        free(this->fl_weights);
        if(this->is_learning){
            free(this->ave_ll_weights);
            free(this->ave_fl_weights);
        }
    }
    
};



void save_model(Model* model,char* filename){
};
void load_model(char* filename){
};
#endif
