#pragma once
#include<vector>

namespace daidai{

typedef int Character;
typedef std::vector<Character> Word;
typedef std::vector<Character> RawSentence;
typedef std::vector<Word> SegmentedSentence;

enum POC{
    POC_B,
    POC_M,
    POC_E,
    POC_S
};

typedef std::vector<POC> POCSequence;
typedef std::vector< std::vector<POC> > POCGraph;

inline int put_character(Character c){
    if(c<128){//1个byte的utf-8
        putchar(c);
    }else if(c<0x800){//2个byte的utf-8
        putchar(0xc0|(c>>6));
        putchar(0x80|(c&0x3f));
    }else if(c<0x10000){//3个byte的utf-8
        putchar(0xe0|((c>>12)&0x0f));
        putchar(0x80|((c>>6)&0x3f));
        putchar(0x80|(c&0x3f));
    }else {//4个byte的utf-8
        putchar(0xf0|((c>>18)&0x07));
        putchar(0x80|((c>>12)&0x3f));
        putchar(0x80|((c>>6)&0x3f));
        putchar(0x80|(c&0x3f));
    };
};

inline int put_word(Word w){
    for(int j=0;j<(int)w.size();j++){
        put_character(w[j]);
    }
}
inline int put_raw(RawSentence& r){
    for(int j=0;j<(int)r.size();j++){
        put_character(r[j]);
    }
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

}
