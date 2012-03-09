#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>

#include "raw.h"

namespace daidai{


typedef Raw Word;
typedef Raw RawSentence;
typedef std::vector<Word> SegmentedSentence;




enum POC{
    kPOC_B='0',
    kPOC_M='1',
    kPOC_E='2',
    kPOC_S='3'
};


typedef std::vector<POC> POCSequence;
typedef std::vector<int> POCGraph;


inline int put_word(Word w,FILE * pFile=stdout){
    for(int j=0;j<(int)w.size();j++){
        put_character(w[j],pFile);
    }
};


struct WordWithTag{
    Word word;
    std::string tag;
    friend std::ostream& operator<< (std::ostream& os,WordWithTag& wwt){
        os<<wwt.word<<"_"<<wwt.tag;
        return os;
    }
};

//typedef std::vector<WordWithTag> TaggedSentence;
class TaggedSentence: public std::vector<WordWithTag>{
    friend std::ostream& operator<< (std::ostream& os,TaggedSentence& sentence){
        for(size_t i=0;i<sentence.size();i++){
            if(i!=0)os<<" ";
            os<<sentence[i];
        }
        return os;    
    };

};


struct LatticeEdge{
    int begin;//begin
    Word word;//string
    std::string tag;//pos tag of the word
    int margin;//margin
    
    friend std::ostream& operator<< (std::ostream& os,LatticeEdge& edge){
        os<<edge.begin<<"_"<<edge.word<<"_"<<edge.tag<<"_"<<edge.margin;
        return os;    
    };
    friend std::istream& operator>> (std::istream& is,LatticeEdge& edge){
        return is;
    };
};

class Lattice: public std::vector<LatticeEdge>{
public:
    static int split_item(std::string& item,int offset,std::string& str){
        int del_ind=item.find_first_of('_',offset);
        str=item.substr(offset,del_ind-offset);
        return del_ind+1;
    };
    friend std::ostream& operator<< (std::ostream& os,Lattice& lattice){
        for(int i=0;i<lattice.size();i++){
            if(i>0)os<<" ";
            os<<lattice[i];
        }
        return os;
    }
    friend std::istream& operator>> (std::istream& is,Lattice& lattice){
        lattice.clear();
        std::string str;
        std::string item;
        std::getline(is,str);

        std::istringstream iss(str);
        while(iss){
            item.clear();
            iss>>item;
            if(item.length()==0)continue;
            lattice.push_back(LatticeEdge());
            LatticeEdge& edge=lattice.back();
            int offset=0;
            std::string begin_str;
            offset=split_item(item,offset,begin_str);
            edge.begin=atoi(begin_str.c_str());
            std::string word_str;
            offset=split_item(item,offset,word_str);
            string_to_raw(word_str,edge.word);
            //edge.word+=word_str;
            offset=split_item(item,offset,edge.tag);
            std::string margin_str;
            offset=split_item(item,offset,margin_str);
            edge.margin=atoi(margin_str.c_str());
            

        }
        return is;
    };

};




template<class KEY>
class Indexer{
public:
    std::map<KEY,int> dict;
    std::vector<KEY> list;
    Indexer(){dict.clear();};
    void save_as(const char* filename);
    int get_index(const KEY& key){
        typename std::map<KEY,int>::iterator it;
        it=dict.find(key);
        if(it==dict.end()){
            int id=(int)dict.size();
            dict[key]=id;
            list.push_back(key);
            return id;
        }else{
            return it->second;
        }
    };
    KEY* get_object(int ind){
        if(ind<0||ind>=dict.size())return NULL;
        return &list[ind];
    }
};

template<class KEY>
class Counter:public std::map<KEY,int>{
public:
    void update(const KEY& key){
        typename std::map<KEY,int>::iterator it;
        it=find(key);
        if(it==this->end()){
            (*this)[key]=0;
        }
        (*this)[key]++;
    }
};

}//end of daidai
