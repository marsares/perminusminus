#pragma once
#include"tagging_decoder.h"
#include<cstdio>
#include<set>
#include<string>

#include<vector>
#include<map>
namespace daidai{




// TaggingDecoder
class TaggingLearner : public  TaggingDecoder {
public:
    Character* gold_standard;
    int T;
    Character separator;
    int seg_only;
    int bigram_threshold;
    void load_tagged_sentence(FILE* file);
    
    
    void train(const char*training_file,
            const char*dat_file,
            const char*model_file,
            const char*label_file);

    TaggingLearner(int T=15,Character separator='/',int seg_only=false){
        this->T=T;
        this->gold_standard=new int[this->max_length];
        this->separator=separator;
        this->seg_only=seg_only;
        bigram_threshold=1;
    };
    ~TaggingLearner(){
        delete this->gold_standard;
    };
    
    Indexer<RawSentence> tag_indexer;
    Indexer<RawSentence> ngram_indexer;
private:
    inline int get_char_tag(const int& poc,const RawSentence& tag){
        RawSentence char_tag;
        char_tag.clear();
        char_tag.push_back(poc);
        if((!seg_only)&&tag.size()){
            char_tag.push_back('/');
            for(size_t j=0;j<tag.size();j++)char_tag.push_back(tag[j]);
        }
        return tag_indexer.get_index(char_tag);
    };
};



}//end of daidai
