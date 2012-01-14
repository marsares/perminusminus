#ifndef __TAGGING_DECODER_H__
#define __TAGGING_DECODER_H__
#include <cstdio>
#include <cstdlib>
#include <string.h>
#include "daidai_base.h"
#include "dat.h"
#include "ngram_feature.h"
#include "../path_labeling/decoder.h"
#include "../path_labeling/model.h"

namespace daidai{


class TaggingDecoder{
public:
    int max_length;
    /*句子*/
    int len;//句子长度
    int* sequence;//句子
    int** allowed_label_lists;///
    int** pocs_to_tags;///

    ///*特征*//
    NGramFeature* ngram_feature;



    ///*双数组*/
    DAT* dat;
    int is_old_type_dat;

    ///*模型参数*/
    permm::Model* model;

    ///*解码用*/
    permm::Node* nodes;//只用来存储解码用的节点的拓扑结构
    int* values;//存各个节点的权重
    permm::Alpha_Beta* alphas;//前向算法数据
    permm::Alpha_Beta* betas;//后向算法数据
    int best_score;
    int* result;//存储结果
    
    char** label_info;


    ///**合法转移矩阵*/
    int*label_trans;
    int**label_trans_pre;
    int**label_trans_post;

    ///*后处理用*/
    int threshold;
    int* allow_com;

    ///*后处理用_ tagging*/
    int tag_size;//postag的个数
    int** label_looking_for;
    int* is_good_choice;
    
    /*构造函数*/
    TaggingDecoder();
    ~TaggingDecoder();
    
    
    /*初始化*/
    void init(const char* model_file,const char* dat_file,const char* label_file,
        char* label_trans=NULL);
    void set_label_trans();//
   
    /*解码*/
    void put_values();
    void dp();
    void cal_betas();
    
    /*接口*/
    int segment(int* input,int length,int* tags);
    int segment(RawSentence&,SegmentedSentence&);
    int segment(RawSentence&,POCGraph&,SegmentedSentence&);
    int segment(RawSentence&,POCGraph&);
    
    void find_good_choice();
    
    /*输入输出*/
    void output_raw_sentence();
    void output_sentence();
    void output_allow_tagging();
    
private:


    void load_label_trans(char*filename);
    
    
};


}

#endif
