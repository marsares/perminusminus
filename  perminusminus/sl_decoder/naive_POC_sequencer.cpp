#include<vector>
#include<cstdio>
#include"daidai_definitions.h"
#include"naive_POC_sequencer.h"




int main(){
    daidai::RawSentence sentence;
    sentence.push_back(12);
    sentence.push_back(24);
    sentence.push_back(48);
    
    daidai::POCGraph poc_graph;
    
    daidai::NaivePOCSequencer sser;

    printf("%d\n",(int)poc_graph.size());
    sser.segment(&sentence,&poc_graph);
    printf("%d\n",(int)poc_graph.size());
    return 1;
};
