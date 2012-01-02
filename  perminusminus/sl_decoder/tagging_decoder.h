#ifndef __TAGGING_DECODER_H__
#define __TAGGING_DECODER_H__
#include <cstdio>
#include <cstdlib>
#include <string.h>

#include "decoder.h"
#include "model.h"

namespace daidai{


class TaggingDecoder{
public:
    int max_length;
    /*句子*/
    int len;//句子长度
    int* sequence;//句子
    int** allowed_label_lists;///

    ///*特征、双数组相关*/
    int* uni_bases;
    int* bi_bases;

    ///*双数组*/
    int dat_size;//双数组大小
    int* bases;//base数组
    int* checks;//check数组

    ///*模型*/
    permm::Model* model;

    ///*解码用*/
    permm::Node* nodes;//只用来存储解码用的节点的拓扑结构
    int* values;
    permm::Alpha_Beta* alphas;
    permm::Alpha_Beta* betas;
    int* result;
    char** label_info;

    ///**字对应的标记*/
    int* char_label_map;
    int* allowed_labels;

    ///**合法转移矩阵*/
    int*label_trans;
    int**label_trans_pre;
    int**label_trans_post;

    ///*后处理用*/
    int threshold;
    int* allow_sep;
    int* allow_com;

    ///*后处理用_ tagging*/
    int tag_size;
    int** label_looking_for;
    int* is_good_choice;
    
    /*构造函数*/
    TaggingDecoder();
    ~TaggingDecoder();
    
    /*双数组trie树相关*/
    void load_da(char* filename,int* &base_array,int* &check_array,int &size);
    
    /*初始化*/
    void init(char* model_file,char* dat_file,char* label_file,
        char* label_trans=NULL,char* label_lists_file=NULL);
    
    void dp();
    void cal_betas();
    void put_values();
    
    
    int segment(int* input,int length,int* tags);
    
    void find_good_choice();
    
    /*输入输出*/
    int get_input_from_stream(int*input,int max_length,int& length);    
    
    void output_raw_sentence();
    void output_sentence();
    void output_allow_tagging();
    void output(int show_sentence);
    
    void set_label_trans();
private:
    /*双数组trie树相关*/
    inline void find_bases(int*bases,int*checks,int dat_size,int ch1,int ch2,int& uni_base,int&bi_base);

    inline void add_values(int *value_offset,int base,int del,int* p_allowed_label);

    void load_label_lists(char*filename);
    void load_label_trans(char*filename);
    
    
};


}

#endif
