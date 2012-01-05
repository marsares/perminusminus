#pragma once
#include<vector>
#include<cstdio>
#include"tagging_decoder.h"
namespace daidai{

//最简单的
class NaivePOCSequencer{
public:
    int segment(RawSentence& sentence,POCGraph& graph){
        graph.clear();
        for(int i=0;i<(int)sentence.size();i++){
            graph.push_back(std::vector<POC>());
            graph[i].push_back(POC_B);
            graph[i].push_back(POC_M);
            graph[i].push_back(POC_E);
            graph[i].push_back(POC_S);
        }
        return 0;
    };
};


/*
基于字标注的过滤器
Averaged Perceptron based POC Sequencer
*/
class APPOCSequencer{
private:
    int input[1000];
    int tags[1000];
    daidai::TaggingDecoder* decoder;
    
public:
    APPOCSequencer(int threshold=15){
        char model_file[]="ctb5/seg.model";
        char dat_file[]="ctb5/seg.dat";
        char label_file[]="ctb5/seg.label_index";
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
//基于启发规则的

}//end of namespace daidai
