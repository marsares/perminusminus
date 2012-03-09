#pragma once
#include<vector>
#include<cstdio>
#include"tagging_decoder.h"
namespace daidai{

class BasePOCSequencer{
    virtual int segment(RawSentence& sentence,POCGraph& graph)=0;
};

//最简单的
class NaivePOCSequencer: public BasePOCSequencer{
public:
    int segment(RawSentence& sentence,POCGraph& graph){
        graph.clear();
        for(int i=0;i<(int)sentence.size();i++){
            graph.push_back(15);
        }
        return 0;
    };
};


/*
基于字标注的过滤器
Averaged Perceptron based POC Sequencer
*/
class APPOCSequencer: public BasePOCSequencer{
private:
    int input[1000];
    int tags[1000];
    daidai::TaggingDecoder* decoder;
    
public:
    APPOCSequencer(int threshold=15){
        char model_file[]="ctb5/segment_model.bin";
        char dat_file[]="ctb5/segment_dat.bin";
        char label_file[]="ctb5/segment_label.txt";
        decoder=new daidai::TaggingDecoder();
        decoder->threshold=threshold*1000;
        decoder->init(model_file,dat_file,label_file);
    };
    ~APPOCSequencer(){
        delete decoder;
    }
    
    int segment(RawSentence& sentence,POCGraph& old_graph,SegmentedSentence& segged){
        decoder->segment(sentence,old_graph,segged);
    }
    
    int segment(RawSentence& sentence,POCGraph& graph){
        decoder->segment(sentence,graph);
    }
};

//基于词典匹配的
class DictPOCSequencer{
private:
    DAT* p_dat;
    
public:
    DictPOCSequencer(const char* filename){
        p_dat=NULL;
        p_dat=new DAT(filename);
        //std::cout<<filename<<"\n";
    }
    ~DictPOCSequencer(){
        delete p_dat;
    }
    int update(RawSentence& sentence,std::vector<int>&poc_cands){
        std::vector<int>bs;
        std::vector<int>es;
        p_dat->search(sentence,bs,es);
        for(int i=0;i<bs.size();i++){
            //std::cout<<bs[i]<<" "<<es[i]<<"\n";
            int b=bs[i];
            int e=es[i];
            if((e-b)==1){//s=8
                poc_cands[b]|=8;
            }else{//b=1 m=2 e=4
                poc_cands[b]|=1;
                poc_cands[e-1]|=4;
                for(int j=b+1;j<e-1;j++){
                    poc_cands[j]|=2;
                }
            }
        }

    }
    int segment(RawSentence& sentence,std::vector<int>&poc_cands){
        poc_cands.clear();
        for(int i=0;i<sentence.size();i++){
            poc_cands.push_back(0);
        }
        update(sentence,poc_cands);        
        /*for(int i=0;i<sentence.size();i++){
            //if(poc_cands[i]==0)poc_cands[i]=15;
            std::cout<<poc_cands[i]<<" ";
        }
        std::cout<<"\n";*/
    }
};

//基于启发规则的

}//end of namespace daidai
