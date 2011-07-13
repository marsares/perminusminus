#ifndef __CORE_H__
#define __CORE_H__
#include<stdio.h>

/*the structure of the model, also the file of the model*/
const int DEC=1000;
struct Model{
    int l_size;//size of the labels
    int f_size;//size of the features
    int* ll_weights;
    int* fl_weights;//weights of (feature,label)
    
    double* ave_ll_weights;
    double* ave_fl_weights;//weights of (feature,label)
    
    
    Model(int l,int f){
        this->l_size=l;
        this->f_size=f;
        this->ll_weights=(int*)calloc(sizeof(int),l*l);
        this->fl_weights=(int*)calloc(sizeof(int),l*f);
        this->ave_ll_weights=(double*)calloc(sizeof(double),l*l);
        this->ave_fl_weights=(double*)calloc(sizeof(double),l*f);
    }
    
    void average(int step){
        int l_size=this->l_size;
        int f_size=this->f_size;
        for(int i=0;i<l_size*f_size;i++){
            this->fl_weights[i]=(int)
                    (
                     (
                        (double)(this->fl_weights[i])
                         -
                        (this->ave_fl_weights[i])/step
                      )
                     *DEC+0.5
                     )
            ;
        }
        for(int i=0;i<l_size*l_size;i++){
            this->ll_weights[i]=(int)
                    (
                        (
                         (double)(this->ll_weights[i])
                         -
                        (this->ave_ll_weights[i])/step   
                        )
                    *DEC+0.5
                    )
            ;
            //printf("%f\n",this->ave_ll_weights[i]);
            //printf("%d\n",this->ll_weights[i]);
        }
    }
    
    Model(char* filename){
        FILE* pFile;
        pFile=fopen(filename,"rb");
        fread(&(this->l_size),4,1,pFile);
        fread(&(this->f_size),4,1,pFile);
        int l_size=this->l_size;
        int f_size=this->f_size;
        //printf("%d %d\n",this->l_size,this->f_size);
        this->ll_weights=(int*)malloc(sizeof(int)*l_size*l_size);
        this->fl_weights=(int*)malloc(sizeof(int)*l_size*f_size);
        
        fread((this->ll_weights),4,l_size*l_size,pFile);
        fread((this->fl_weights),4,l_size*f_size,pFile);
        fclose(pFile);
    }
    
    void save(char* filename){
        FILE* pFile=fopen(filename,"wb");
        int l_size=this->l_size;
        int f_size=this->f_size;
        
        fwrite(&(this->l_size),4,1,pFile);
        fwrite(&(this->f_size),4,1,pFile);
        //printf("%d %d\n",l_size,f_size);
        fwrite((this->ll_weights),4,l_size*l_size,pFile);
        fwrite((this->fl_weights),4,l_size*f_size,pFile);
        fclose(pFile);
    }
    ~Model(){
        free(this->ll_weights);
        free(this->fl_weights);
        if((this->ave_ll_weights)!=NULL){
            free(this->ave_ll_weights);
            free(this->ave_fl_weights);
        }
    }
    
};



#endif
