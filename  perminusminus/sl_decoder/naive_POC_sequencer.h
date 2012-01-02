#pragma once
#include<vector>
#include"daidai_definitions.h"

namespace daidai{

class NaivePOCSequencer{
public:
    int segment(RawSentence* sentence,POCGraph* graph){
        graph->clear();
        for(int i=0;i<(int)sentence->size();i++){
            graph->push_back(std::vector<POC>());
            (*graph)[i].push_back(POC_B);
            (*graph)[i].push_back(POC_M);
            (*graph)[i].push_back(POC_E);
            (*graph)[i].push_back(POC_S);
        }
        return 0;
    };
};
}
