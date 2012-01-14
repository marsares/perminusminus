#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <algorithm> 

#include "tagging_learner.h"
#include "ngram_feature.h"
#include "dat.h"
using namespace daidai;


class TaggedFileLoader{
private:    
    std::string str;
    std::string item;

    char del;
    RawSentence tag;
    RawSentence word;

    RawSentence char_tag;
public:

    struct WordAndTag{
        Word word;
        Word tag;
    };

    std::ifstream* ifs;
    TaggedFileLoader(const char* filename){
        del='_';
        ifs=new std::ifstream(filename,std::ifstream::in);
    };
    void load(std::vector<WordAndTag>& seq){
        seq.clear();
        getline((*ifs),str);
        std::istringstream iss(str);
        while(iss){
            item.clear();
            iss>>item;
            if(!item.length())continue;
            int del_ind=item.find_first_of(del);

            const std::string&tag_string=item.substr(del_ind+1,item.length());
            const std::string&word_string=item.substr(0,del_ind);
            seq.push_back(WordAndTag());
            
            string_to_raw(tag_string,seq.back().tag);
            string_to_raw(word_string,seq.back().word);
            
        }
        
    };
    ~TaggedFileLoader(){
        delete ifs;
    };

};


bool compare_words (DATMaker::KeyValue first, DATMaker::KeyValue second)
{
    daidai::Word& first_word=first.key;
    daidai::Word& second_word=second.key;
    size_t min_size=(first_word.size()<second_word.size())?first_word.size():second_word.size();
    for(int i=0;i<min_size;i++){
        if(first_word[i]>second_word[i])return false;
        if(first_word[i]<second_word[i])return true;
    }
    
  return (first_word.size()<second_word.size());
}




void TaggingLearner::train(const char*training_file,
        const char*model_file,
        const char*dat_file,
        const char*label_file){
    

    RawSentence raw;
    RawSentence char_tag;
    NGramFeature ngram_feature;
    Counter<Word> bigram_counter;
    Word bigram;
    
    std::vector<TaggedFileLoader::WordAndTag> sent;
    TaggedFileLoader* tfl=new TaggedFileLoader(training_file);
    while((*(tfl->ifs))){
        tfl->load(sent);
        if(!sent.size())continue;
        raw.clear();
        for(int i=0;i<sent.size();i++){
            const RawSentence& word=sent[i].word;
            const RawSentence& tag=sent[i].tag;
            for(int j=0;j<word.size();j++)raw.push_back(word[j]);
            
            if(word.size()==1){
                get_char_tag('3',tag);
            }else{
                get_char_tag('0',tag);
                if(word.size()>2){
                    get_char_tag('1',tag);
                }
                get_char_tag('2',tag);
            }
        }
        if(bigram_threshold>1){
            for(int i=0;i<raw.size()-1;i++){
                bigram.clear();
                bigram.push_back(raw[i]);bigram.push_back(raw[i+1]);
                bigram_counter.update(bigram);
            }
        }
        ngram_feature.feature_generation(raw,ngram_indexer);
        
    }
    delete tfl;
    
    
    //dat_file

    std::vector<DATMaker::KeyValue> kv;
    int feature_ind=0;
    for(int i=0;i<ngram_indexer.list.size();i++){
        const Word& feature_raw=ngram_indexer.list[i];
        //filter some bigrams
        if((bigram_threshold>1)&&(feature_raw.size()>=2)){
            if((feature_raw[0]!=' ')&&(feature_raw[1]!=' ')
                    &&(feature_raw[0]!='#')&&(feature_raw[1]!='#')){
                bigram.clear();
                bigram.push_back(feature_raw[0]);bigram.push_back(feature_raw[1]);
                if(bigram_counter[bigram]<bigram_threshold){
                    //put_character(bigram[0]);put_character(bigram[1]);putchar('\n');
                    continue;
                }
            }
        }
        kv.push_back(DATMaker::KeyValue());
        kv.back().key=feature_raw;
        kv.back().value=feature_ind++;
        
    }
    
    std::sort(kv.begin(),kv.end(),compare_words);
    
    DATMaker* dm=new DATMaker();
    dm->make_dat(kv,1);
    dm->shrink();
    dm->save_as(dat_file);
    delete dm;
    fprintf(stderr,"dat file created\n");
    
    
    //model_file
    int l_size=tag_indexer.list.size();
    int f_size=kv.size();
    std::cout<<l_size<<"\n";
    std::cout<<f_size<<"\n";
    permm::Model* model=new permm::Model(l_size,f_size);
    model->save(model_file);
    delete model;
    fprintf(stderr,"model file created\n");
    
    
    
    
    //label_file
    FILE * pFile=fopen(label_file,"w");
    for(int i=0;i<tag_indexer.list.size();i++){
        put_raw(tag_indexer.list[i],pFile);
        fputc('\n',pFile);
    }
    fclose(pFile);
    fprintf(stderr,"label file created\n");
    
    //init decoder
    init(model_file,dat_file,label_file);
    this->model->reset_ave_weights();
    fprintf(stderr,"decoder initialized\n");
    
    //learning !!!
    
    long steps=0;
    for(int t=0;t<this->T;t++){
        fprintf(stderr,"iteration %d\n",t+1);
        int number_nodes=0;
        int number_correct=0;
        tfl=new TaggedFileLoader(training_file);
        while((*(tfl->ifs))){
            tfl->load(sent);
            if(!sent.size())continue;
            steps++;
            len=0;
            
            
            
            for(int i=0;i<sent.size();i++){
                const RawSentence& word=sent[i].word;
                const RawSentence& tag=sent[i].tag;
                
                for(int j=0;j<word.size();j++){
                    
                    this->sequence[len]=word[j];
                    if(word.size()==1){
                        gold_standard[len]=get_char_tag('3',tag);
                    }else{
                        if(j==0){
                            gold_standard[len]=get_char_tag('0',tag);
                        }else if((j+1)==word.size()){
                            gold_standard[len]=get_char_tag('2',tag);
                        }else{
                            gold_standard[len]=get_char_tag('1',tag);
                        }
                    }
                    len++;
                }
                
            }

            //decode
            put_values();
            dp();

            
            //update
            this->ngram_feature->update_weights(sequence,len,gold_standard,1,steps);
            this->ngram_feature->update_weights(sequence,len,result,-1,steps);
            for(int i=0;i<len-1;i++){
                this->model->update_ll_weight(gold_standard[i],gold_standard[i+1],1,steps);
                this->model->update_ll_weight(result[i],result[i+1],-1,steps);
            }

            
            for(int i=0;i<len;i++){
                
                number_nodes++;
                if(gold_standard[i]==result[i])number_correct++;
            }
            
        }
        std::cout<<number_correct<<" "<<number_nodes<<" "<<(double)number_correct/number_nodes<<"\n";
        delete tfl;
    }
    //average
    this->model->average(steps);
    //save model
    this->model->save(model_file);
}


