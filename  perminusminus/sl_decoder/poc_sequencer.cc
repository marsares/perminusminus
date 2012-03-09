#include<vector>
#include<cstdio>
#include"poc_sequencer.h"




int main(){
    
    //daidai::RawSentence sentence;
    //sentence.push_back(12);
    //sentence.push_back(24);
    //sentence.push_back(48);
    
    //daidai::POCGraph poc_graph;
    //daidai::POCGraph poc_graph2;
    
    //daidai::NaivePOCSequencer sser;
    
    //daidai::APPOCSequencer ap(30);
    
    //int end=daidai::get_raw(sentence);
    //daidai::put_raw(sentence);
    //putchar('\n');
    //sser.segment(sentence,poc_graph);
    
    //daidai::SegmentedSentence segged;
    //ap.segment(sentence,poc_graph,segged);
    //for(int i=0;i<(int)segged.size();i++){
        //daidai::put_word(segged[i]);
        //putchar(' ');
    //};putchar('\n');
    
    //ap.segment(sentence,poc_graph2);
    //for(int i=0;i<(int)poc_graph2.size();i++){
        //for(int j=0;j<(int)poc_graph2[i].size();j++){
            //printf("%d ",poc_graph2[i][j]);
        //}printf("\n");
        
    //};putchar('\n');
    
    daidai::DictPOCSequencer d_s("dict.bin");
    
    daidai::RawSentence sentence;
    daidai::POCGraph poc_cands;
    daidai::POCGraph poc_graph2;
    daidai::APPOCSequencer ap(30);
    daidai::SegmentedSentence segged;
    
    int end=daidai::get_raw(sentence);
    daidai::put_raw(sentence);
    putchar('\n');
    d_s.segment(sentence,poc_cands);
    
    ap.segment(sentence,poc_cands,segged);
    for(int i=0;i<(int)segged.size();i++){
        daidai::put_word(segged[i]);
        putchar(' ');
    };putchar('\n');
    
    return 0;
};
