#pragma once
#include<cstdio>
#include<iostream>
#include"dat.h"
#include"../path_labeling/model.h"
#include<map>
namespace daidai{//beginning of daidai

/*
what we need:
a model, a DAT
*/
class NGramFeature{
private:

    int max_length;
    ///*特征、双数组相关*/
    int* uni_bases;
    int* bi_bases;

    int* values;
    ///*双数组*/
    int dat_size;//双数组大小
    DAT::Entry* dat;

    permm::Model* model;

    //std::map<Word,int> features;
    
public: 
    NGramFeature(){
        uni_bases=NULL;
        bi_bases=NULL;
    };
    NGramFeature(DAT* dat,permm::Model* model,int* values){
        this->dat=dat->dat;
        this->dat_size=dat->dat_size;
        this->model=model;
        max_length=1000;
        this->uni_bases=new int[this->max_length+2];
        this->bi_bases=new int[this->max_length+4];
        this->values=values;
    };
    ~NGramFeature(){
        if(uni_bases)delete[] uni_bases;
        if(bi_bases)delete[] bi_bases;
    };

    inline void feature_generation(RawSentence& seq,Indexer<RawSentence>& indexer){
        int mid=0;
        int left=0;int left2=0;
        int right=0;int right2=0;
        RawSentence key;
        
        for(int i=0;i<seq.size();i++){
            mid=seq[i];
            left=(i>0)?(seq[i-1]):('#');
            left2=((i-2)>=0)?(seq[i-2]):('#');
            right=((i+1)<seq.size())?(seq[i+1]):('#');
            right2=((i+2)<seq.size())?(seq[i+2]):('#');
            
            key.clear();
            key.push_back(mid);key.push_back(' ');key.push_back('1');
            indexer.get_index(key);
            key.clear();
            key.push_back(left);key.push_back(' ');key.push_back('2');
            indexer.get_index(key);
            key.clear();
            key.push_back(right);key.push_back(' ');key.push_back('3');
            indexer.get_index(key);
            
            key.clear();
            key.push_back(left);key.push_back(mid);key.push_back(' ');key.push_back('1');
            indexer.get_index(key);
            key.clear();
            key.push_back(mid);key.push_back(right);key.push_back(' ');key.push_back('2');
            indexer.get_index(key);
            key.clear();
            key.push_back(left2);key.push_back(left);key.push_back(' ');key.push_back('3');
            indexer.get_index(key);
            key.clear();
            key.push_back(right);key.push_back(right2);key.push_back(' ');key.push_back('4');
            indexer.get_index(key);
        }
    };

    int put_values(int*sequence,int len){
        find_bases(dat_size,35,35,uni_bases[0],bi_bases[0]);
        find_bases(dat_size,35,sequence[0],uni_bases[0],bi_bases[1]);
        for(int i=0;i+1<len;i++)
            find_bases(dat_size,sequence[i],sequence[i+1],uni_bases[i+1],bi_bases[i+2]);
        find_bases(dat_size,sequence[len-1],35,uni_bases[len],bi_bases[len+1]);
        find_bases(dat_size,35,35,uni_bases[len+1],bi_bases[len+2]);
        
        int base=0;
        for(int i=0;i<len;i++){
            int* value_offset=values+i*model->l_size;
            if((base=uni_bases[i+1])!=-1)
                add_values(value_offset,base,49,NULL);
            if((base=uni_bases[i])!=-1)
                add_values(value_offset,base,50,NULL);
            if((base=uni_bases[i+2])!=-1)
                add_values(value_offset,base,51,NULL);
            if((base=bi_bases[i+1])!=-1)
                add_values(value_offset,base,49,NULL);
            if((base=bi_bases[i+2])!=-1)
                add_values(value_offset,base,50,NULL);
            if((base=bi_bases[i])!=-1)
                add_values(value_offset,base,51,NULL);
            if((base=bi_bases[i+3])!=-1)
                add_values(value_offset,base,52,NULL);
        }

    };
    int update_weights(int*sequence,int len,int* results,int delta,long steps){
        find_bases(dat_size,35,35,uni_bases[0],bi_bases[0]);
        find_bases(dat_size,35,sequence[0],uni_bases[0],bi_bases[1]);
        for(int i=0;i+1<len;i++)
            find_bases(dat_size,sequence[i],sequence[i+1],uni_bases[i+1],bi_bases[i+2]);
        find_bases(dat_size,sequence[len-1],35,uni_bases[len],bi_bases[len+1]);
        find_bases(dat_size,35,35,uni_bases[len+1],bi_bases[len+2]);
        
        int base=0;
        for(int i=0;i<len;i++){
            int* value_offset=values+i*model->l_size;
            if((base=uni_bases[i+1])!=-1)
                update_weight(value_offset,base,49,results[i],delta,steps);
            if((base=uni_bases[i])!=-1)
                update_weight(value_offset,base,50,results[i],delta,steps);
            if((base=uni_bases[i+2])!=-1)
                update_weight(value_offset,base,51,results[i],delta,steps);
            if((base=bi_bases[i+1])!=-1)
                update_weight(value_offset,base,49,results[i],delta,steps);
            if((base=bi_bases[i+2])!=-1)
                update_weight(value_offset,base,50,results[i],delta,steps);
            if((base=bi_bases[i])!=-1)
                update_weight(value_offset,base,51,results[i],delta,steps);
            if((base=bi_bases[i+3])!=-1)
                update_weight(value_offset,base,52,results[i],delta,steps);
        }

    }
private:
    inline void update_weight(int *value_offset,int base,int del,int label,int delta,long steps){
        int ind=dat[base].base+del;
        if(ind>=dat_size||dat[ind].check!=base)return;
        register int offset=dat[dat[base].base+del].base;
        model->update_fl_weight(offset,label,delta,steps);
        //model->fl_weights[offset*model->l_size+label]+=delta;
    }
    /*只内部调用*/
    inline void add_values(int *value_offset,int base,int del,int* p_allowed_label=NULL){
        int ind=dat[base].base+del;
        if(ind>=dat_size||dat[ind].check!=base){
            return;
        }
        int offset=dat[dat[base].base+del].base;
        int* weight_offset=model->fl_weights+offset*model->l_size;
        int allowed_label;
        if(model->l_size==4){
            value_offset[0]+=weight_offset[0];
            value_offset[1]+=weight_offset[1];
            value_offset[2]+=weight_offset[2];
            value_offset[3]+=weight_offset[3];
        }else{
            if(p_allowed_label){
                while((allowed_label=(*(p_allowed_label++)))>=0){
                    value_offset[allowed_label]+=weight_offset[allowed_label];
                }
            }else{
                for(int j=0;j<model->l_size;j++){
                    value_offset[j]+=weight_offset[j];
                }
            }
        }
    };

    /*
     * 找出以ch1 ch2为字符的dat的下标
     * */
    inline void find_bases(int dat_size,int ch1,int ch2,int& uni_base,int&bi_base){
        if(dat[ch1].check){
            uni_base=-1;bi_base=-1;return;
        }
        uni_base=dat[ch1].base+32;
        int ind=dat[ch1].base+ch2;
        if(ind>=dat_size||dat[ind].check!=ch1){
            bi_base=-1;return;
        }
        bi_base=dat[ind].base+32;
    }


};


}//end of daidai
