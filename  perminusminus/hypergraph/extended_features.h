#pragma once
#include "lattice.h"

namespace daidai{
namespace hypergraph{
/**
 * 节点的扩展特征
 * 1 词典信息，带词性
 * 2 SogouT的RAV
 * 3 SogouT的新信息
 * 4 串的命名体生成概率
 * */
class SogouTFeature : public NodeFeature{
public:
    DAT* dat;
public:
    SogouTFeature(DAT* dat){
        this->dat=dat;
    };
    ~SogouTFeature(){};
    void add_features(Graph::Node& node,std::vector<Raw>& keys){
        int base=dat->match(node.data.word);
        if(base>0){
            int word_base=dat->dat[dat->dat[base].check].base;
            int count=dat->dat[base].base;
            //std::cout<<node.data.word<<" ";
            //std::cout<<count<<"\n";
            //if(count>10)return;

            keys.push_back(Raw());
            Raw& key=keys.back();
            key+=node.data.tag;
            key.push_back(' ');
            key.push_back(count+'a');
            int rav=count/3+'a';
            
            //return;
            word_base=dat->dat[word_base+'_'].base;
            count=dat->dat[word_base].base;
            //std::cout<<count<<"\n";
            //return;
            //if(count>10)count=10;
            for(int i=0;i<count;i++){
                int tag_ind=dat->dat[dat->dat[word_base+i+19968].base].base;
                //std::cout<<tag_ind<<"\n";
                keys.push_back(Raw());
                Raw& raw=keys.back();
                raw+=node.data.tag;
                raw.push_back(' ');
                raw.push_back(tag_ind);
                raw.push_back(rav);
            }
        }
        //keys.push_back();
        
    };
};
class DictNodeFeature : public NodeFeature{
    /*类似于SogouW的词典信息*/
public:
    DAT* dat;
public:
    DictNodeFeature(DAT* dat){
        this->dat=dat;

    };
    ~DictNodeFeature(){

    };

    void add_features(Graph::Node& node,std::vector<Raw>& keys){
        int base=dat->match(node.data.word);
        if(base>0){
            //std::cout<<node.data.word<<" ";
            int word_base=dat->dat[dat->dat[base].check].base;
            int count=dat->dat[base].base;
            //std::cout<<count<<"\n";
            if(count>10)return;

            //keys.push_back(Raw());

            //keys.back()+=node.data.tag;
            //return;

            for(int i=0;i<count;i++){
                int tag_ind=dat->dat[dat->dat[word_base+i+'1'].base].base;
                //std::cout<<(char)tag_ind<<"\n";
                keys.push_back(Raw());
                Raw& raw=keys.back();
                raw+=node.data.tag;
                if(tag_ind<'a' || tag_ind>'o'){
                    std::cout<<node.data.word<<"\n";
                    std::cout<<i<<" "<<(char)tag_ind<<" "<<tag_ind<<" oh\n";
                }
                raw.push_back(' ');
                raw.push_back(tag_ind);
            }
        }
        //keys.push_back();
        
    };
};

}//end of daidai
}//end of daidai
