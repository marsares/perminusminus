#pragma once
#include<cstdio>
#include<sys/mman.h>
#include<fcntl.h>
namespace permm{

/*the structure of the model, also the file of the model*/
template<typename ValueType=int>
class BasicModel{
public:
    void* mmap_ptr;
    static const int DEC=1000;
    int l_size;//size of the labels
    int f_size;//size of the features
    int* ll_weights;
    int* fl_weights;//weights of (feature,label)
    
    double* ave_ll_weights;
    double* ave_fl_weights;//weights of (feature,label)
    
    
    BasicModel(int l,int f):ave_ll_weights(NULL),ave_fl_weights(NULL){
        this->mmap_ptr=NULL;
        this->l_size=l;
        this->f_size=f;
        this->ll_weights=(int*)calloc(sizeof(int),l*l);
        this->fl_weights=(int*)calloc(sizeof(int),l*f);
        this->ave_ll_weights=(double*)calloc(sizeof(double),l*l);
        this->ave_fl_weights=(double*)calloc(sizeof(double),l*f);
    }
    ~BasicModel(){
        if(mmap_ptr){
            munmap(mmap_ptr,sizeof(*ll_weights)*l_size*(f_size+l_size)+8);
        }else{
            free(this->ll_weights);
            free(this->fl_weights);
        }
        free(this->ave_ll_weights);
        free(this->ave_fl_weights);
        
    }
    void reset_ave_weights(){
        free(this->ave_ll_weights);
        free(this->ave_fl_weights);
        this->ave_ll_weights=(double*)calloc(sizeof(double),l_size*l_size);
        this->ave_fl_weights=(double*)calloc(sizeof(double),l_size*f_size);
    }
    
    void update_ll_weight(const int& i,const int& j,const int& delta,const long& steps){
        int ind=i*l_size+j;
        //std::cout<<i<<" "<<j<<" "<<ind<<"\n";
        this->ll_weights[ind]+=delta;
        this->ave_ll_weights[ind]+=steps*delta;
    }
    void update_fl_weight(const int& i,const int& j,const int& delta,const long& steps){
        int ind=i*l_size+j;
        this->fl_weights[ind]+=delta;
        this->ave_fl_weights[ind]+=steps*delta;
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
        }
    }
    
    BasicModel(const char* filename,int use_mmap=true):ave_ll_weights(NULL),ave_fl_weights(NULL){
        FILE* pFile;
        int rtn_value;
        pFile=fopen(filename,"rb");
        rtn_value=fread(&(this->l_size),4,1,pFile);
        rtn_value=fread(&(this->f_size),4,1,pFile);
        int l_size=this->l_size;
        int f_size=this->f_size;
        //use_mmap=true;
        if(use_mmap){
            fclose(pFile);
            int fd=open(filename,O_RDONLY);
            //printf("fd %d\n",fd);
            mmap_ptr=mmap(NULL,sizeof(int)*l_size*(l_size+f_size),PROT_READ,MAP_SHARED,fd,0);
            //printf("start %d\n",(int)(size_t)mmap_ptr);
            close(fd);
            ll_weights=((int*)mmap_ptr)+2;
            fl_weights=((int*)mmap_ptr)+l_size*l_size+2;
            //printf("flweight %d\n",fl_weights[l_size*(f_size-1)]);

        }else{
            this->ll_weights=(int*)malloc(sizeof(int)*l_size*l_size);
            this->fl_weights=(int*)malloc(sizeof(int)*l_size*f_size);
            
            rtn_value=fread((this->ll_weights),4,l_size*l_size,pFile);
            rtn_value=fread((this->fl_weights),4,l_size*f_size,pFile);
            fclose(pFile);

        }
    }
    
    void save(const char* filename){
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
    
    
};


typedef BasicModel<> Model;

}
