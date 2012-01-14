#pragma once
#include<vector>
#include<map>
#include<string>

namespace daidai{

typedef int Character;
typedef std::vector<Character> Word;
typedef std::vector<Character> RawSentence;
typedef std::vector<Word> SegmentedSentence;

/*
enum POC{
    kPOC_B='B',
    kPOC_M='M',
    kPOC_E='E',
    kPOC_S='S'
};
*/
enum POC{
    kPOC_B='0',
    kPOC_M='1',
    kPOC_E='2',
    kPOC_S='3'
};


typedef std::vector<POC> POCSequence;
typedef std::vector< std::vector<POC> > POCGraph;

inline int put_character(Character c,FILE * pFile=stdout){
    if(c<128){//1个byte的utf-8
        fputc(c,pFile);
    }else if(c<0x800){//2个byte的utf-8
        fputc(0xc0|(c>>6),pFile);
        fputc(0x80|(c&0x3f),pFile);
    }else if(c<0x10000){//3个byte的utf-8
        fputc(0xe0|((c>>12)&0x0f),pFile);
        fputc(0x80|((c>>6)&0x3f),pFile);
        fputc(0x80|(c&0x3f),pFile);
    }else {//4个byte的utf-8
        fputc(0xf0|((c>>18)&0x07),pFile);
        fputc(0x80|((c>>12)&0x3f),pFile);
        fputc(0x80|((c>>6)&0x3f),pFile);
        fputc(0x80|(c&0x3f),pFile);
    };
};


inline int put_word(Word w,FILE * pFile=stdout){
    for(int j=0;j<(int)w.size();j++){
        put_character(w[j],pFile);
    }
}
inline int put_raw(RawSentence& r,FILE * pFile=stdout){
    for(size_t j=0;j<r.size();j++){
        put_character(r[j],pFile);
    }
}

inline int string_to_raw(const std::string& str,RawSentence& sent){
    sent.clear();
    int current_character=-1;
    int c;
    for(int i=0;i<str.length();i++){
        c=str.at(i);
        if(!(c&0x80)){//1个byte的utf-8编码
            if(current_character!=-1)sent.push_back(current_character);
            current_character=c;//+65248;//半角转全角，放入缓存
        }else if(!(c&0x40)){//not a beginning of a Character
            current_character=(current_character<<6)+(c&0x3f);
        }else if(!(c&0x20)){//2个byte的utf-8编码
            if(current_character!=-1)sent.push_back(current_character);
            current_character=(c&0x1f);
        }else if(!(c&0x10)){//3个byte的utf-8编码
            if(current_character!=-1)sent.push_back(current_character);
            current_character=(c&0x0f);
        }else if(!(c&0x80)){//4个byte的utf-8编码
            if(current_character!=-1)sent.push_back(current_character);
            current_character=(c&0x07);
        }else{//更大的unicode编码不能处理
            if(current_character!=-1)sent.push_back(current_character);
            current_character=0;
        }
    }
    if(current_character>0)sent.push_back(current_character);
    return 0;
}

inline int get_raw(RawSentence& sent,FILE* pFile=stdin,int min_char=33){
    sent.clear();
    int current_character=-1;
    int c;
    while(1){//反复读取输入流
        c=fgetc(pFile);
        if(c==EOF){
            if(current_character!=-1)sent.push_back(current_character);
            return c;//end of file
        }
        if(!(c&0x80)){//1个byte的utf-8编码
            if(current_character!=-1)sent.push_back(current_character);
            if(c<min_char){//非打印字符及空格
                return c;
            }else{//一般ascii字符
                current_character=c;//+65248;//半角转全角，放入缓存
            }
        }else if(!(c&0x40)){//not a beginning of a Character
            current_character=(current_character<<6)+(c&0x3f);
        }else if(!(c&0x20)){//2个byte的utf-8编码
            if(current_character!=-1)sent.push_back(current_character);
            current_character=(c&0x1f);
        }else if(!(c&0x10)){//3个byte的utf-8编码
            if(current_character!=-1)sent.push_back(current_character);
            current_character=(c&0x0f);
        }else if(!(c&0x80)){//4个byte的utf-8编码
            if(current_character!=-1)sent.push_back(current_character);
            current_character=(c&0x07);
        }else{//更大的unicode编码不能处理
            if(current_character!=-1)sent.push_back(current_character);
            current_character=0;
        }
    }
}
inline int get_raw(Character* seq,int max_len,int&len,FILE* pFile=stdin,int min_char=33){
    len=0;
    Character current_character=-1;
    int c;
    while(1){//反复读取输入流
        
        c=fgetc(pFile);
        if(c==EOF){
            if((current_character!=-1)&&(len<max_len))seq[len++]=current_character;
            return c;//end of file
        }
        if(!(c&0x80)){//1个byte的utf-8编码
            if((current_character!=-1)&&(len<max_len))seq[len++]=current_character;
            if(c<min_char){//非打印字符及空格
                return c;
            }else{//一般ascii字符
                current_character=c;//+65248;//半角转全角，放入缓存
            }
        }else if(!(c&0x40)){//not a beginning of a Character
            current_character=(current_character<<6)+(c&0x3f);
        }else if(!(c&0x20)){//2个byte的utf-8编码
            if((current_character!=-1)&&(len<max_len))seq[len++]=current_character;
            current_character=(c&0x1f);
        }else if(!(c&0x10)){//3个byte的utf-8编码
            if((current_character!=-1)&&(len<max_len))seq[len++]=current_character;
            current_character=(c&0x0f);
        }else if(!(c&0x80)){//4个byte的utf-8编码
            if((current_character!=-1)&&(len<max_len))seq[len++]=current_character;
            current_character=(c&0x07);
        }else{//更大的unicode编码不能处理
            if((current_character!=-1)&&(len<max_len))seq[len++]=current_character;
            current_character=0;
        }
    }
}


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
