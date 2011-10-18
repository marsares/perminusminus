#ifndef __SL_DECODER_H__
#define __SL_DECODER_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "decoder.h"
#include "model.h"

/*缓存大小*/
const int max_length=10000;

/*句子*/
int len=0;//句子长度
int sequence[max_length];//句子
int* allowed_label_lists[max_length];///

/*特征、双数组相关*/
int uni_bases[max_length+2];
int bi_bases[max_length+4];

/*双数组*/
int dat_size;//双数组大小
int* bases;//base数组
int* checks;//check数组

/*模型*/
Model* model;

/*解码用*/
Node nodes[max_length];//只用来存储解码用的节点的拓扑结构
int* values;
Alpha_Beta* alphas;
Alpha_Beta* betas;
int* result;
char** label_info;

/**字对应的标记*/
int char_label_map[65536];
int* allowed_labels=NULL;

/**合法转移矩阵*/
int*label_trans=NULL;
int**label_trans_pre;
int**label_trans_post;

/*后处理用*/
int threshold=10000;
int allow_sep[max_length];
int allow_com[max_length];

/*后处理用_ tagging*/
int tag_size=0;
int** label_looking_for;
int* is_good_choice;


/*初始化一个双数组trie树*/
void load_da(char* filename,int* &base_array,int* &check_array,int &size){
    //打开文件
    FILE * pFile=fopen ( filename , "rb" );
    /*得到文件大小*/
    fseek (pFile , 0 , SEEK_END);
    size=ftell (pFile)/sizeof(int)/2;//双数组大小
    rewind (pFile);//重置文件指针到开头
    /*前一半是base数组，后一半是check数组*/
    base_array=(int*) malloc (sizeof(int)*size);
    check_array=(int*) malloc (sizeof(int)*size);
    fread (base_array,sizeof(int),size,pFile);
    fread (check_array,sizeof(int),size,pFile);
    //关闭文件
    fclose (pFile);
};

inline void find_bases(int*bases,int*checks,int dat_size,int ch1,int ch2,int& uni_base,int&bi_base){
    if(checks[ch1]){
        uni_base=-1;bi_base=-1;return;
    }
    uni_base=bases[ch1]+32;
    int ind=bases[ch1]+ch2;
    if(ind>=dat_size||checks[ind]!=ch1){
        bi_base=-1;return;
    }
    bi_base=bases[ind]+32;
}


void dp(){
    dp_decode(
            model->l_size,//check
            model->ll_weights,//check
            len,//check
            nodes,
            values,
            alphas,
            result,
            label_trans_pre,
            allowed_label_lists
        );
}
void cal_betas(){
    int tmp=nodes[len-1].successors[0];
    nodes[len-1].successors[0]=-1;
    dp_cal_betas(
            model->l_size,
            model->ll_weights,
            len,
            nodes,
            values,
            betas
    );
    nodes[len-1].successors[0]=tmp;
}

void load_label_lists(char*filename){
    //打开文件
    FILE * pFile=fopen ( filename , "rb" );
    /*得到文件大小*/
    int remain_size=0;
    fread (&remain_size,sizeof(int),1,pFile);
    
    fread (char_label_map,sizeof(int),65536,pFile);
    allowed_labels=new int[remain_size-65536];
    fread (allowed_labels,sizeof(int),remain_size-65536,pFile);
    fclose (pFile);
    //printf("loaded");
    return;
}

void load_label_trans(char*filename){
    //打开文件
    FILE * pFile=fopen ( filename , "rb" );
    /*得到文件大小*/
    int remain_size=0;
    fread (&remain_size,sizeof(int),1,pFile);
    /*得到矩阵数据*/
    label_trans=new int[remain_size];
    fread (label_trans,sizeof(int),remain_size,pFile);
    
    /*计算标签个数*/
    int label_size=0;
    for(int i=0;i<remain_size;i++){
        if(label_trans[i]==-1)label_size++;
    }
    label_size/=2;
    /*设定各个标签的指针*/
    label_trans_pre=new int*[label_size];
    label_trans_post=new int*[label_size];
    int ind=0;
    for(int i=0;i<label_size;i++){
        label_trans_pre[i]=label_trans+ind;
        while(label_trans[ind]!=-1)ind++;ind++;
        label_trans_post[i]=label_trans+ind;
        while(label_trans[ind]!=-1)ind++;ind++;
    }
    //printf("%d",remain_size);
    fclose (pFile);
    
    return;
}

void init(
        char* model_file,
        char* dat_file,
        char* label_file,
        char* label_trans=NULL,
        char* label_lists_file=NULL){
    /**模型*/
    model=new Model(model_file);
    
    /**解码用*/
    values=(int*)calloc(sizeof(int),max_length*model->l_size);
    alphas=(Alpha_Beta*)calloc(sizeof(Alpha_Beta),max_length*model->l_size);
    betas=(Alpha_Beta*)calloc(sizeof(Alpha_Beta),max_length*model->l_size);
    result=(int*)calloc(sizeof(int),max_length*model->l_size);
    label_info=new char*[model->l_size];
    
    for(int i=0;i<max_length;i++){
        int* pr=new int[2];
        pr[0]=i-1;
        pr[1]=-1;
        nodes[i].predecessors=pr;
        
        pr=new int[2];
        pr[0]=i+1;
        pr[1]=-1;
        nodes[i].successors=pr;
    };
    load_da(dat_file,bases,checks,dat_size);
    
    char* str=new char[16];
    FILE *fp;
    fp = fopen(label_file, "r");
    int ind=0;
    while( fscanf(fp, "%s", str)==1){
        label_info[ind]=str;
        str=new char[16];
        ind++;
    }
    fclose(fp);
    
    label_looking_for=new int*[model->l_size];
    for(int i=0;i<model->l_size;i++)
        label_looking_for[i]=NULL;
    for(int i=0;i<model->l_size;i++){
        if(label_info[i][0]=='0' || label_info[i][0]=='3')continue;
        
        for(int j=0;j<=i;j++){
            if((strcmp(label_info[i]+1,label_info[j]+1)==0)&&(label_info[j][0]=='0')){
                if(label_looking_for[j]==NULL){
                    label_looking_for[j]=new int[2];
                    label_looking_for[j][0]=-1;label_looking_for[j][1]=-1;
                    tag_size++;
                }
                label_looking_for[j][label_info[i][0]-'1']=i;
                break;
            }
        }
    }
    //printf("tagsize %d",tag_size);
    

    
    /**label_trans*/
    if(label_trans){
        load_label_trans(label_trans);
    }
    
    if(label_lists_file){
        load_label_lists(label_lists_file);
        
    }else{
        for(int i=0;i<max_length;i++)
            allowed_label_lists[i]=NULL;
    }
    
    is_good_choice=new int[max_length*model->l_size];
    
}
void output_raw_sentence(){
    int c;
    for(int i=0;i<len;i++){
        c=sequence[i];
        if(c<128){//1个byte的utf-8
            putchar(c);
        }else if(c<0x800){//2个byte的utf-8
            putchar(0xc0|(c>>6));
            putchar(0x80|(c&0x3f));
        }else{//3个byte的utf-8
            putchar(0xe0|((c>>12)&0x0f));
            putchar(0x80|((c>>6)&0x3f));
            putchar(0x80|(c&0x3f));
        }
    }
}
void output_sentence(){
    int c;
    for(int i=0;i<len;i++){
        c=sequence[i];
        if(c<128){//1个byte的utf-8
            putchar(c);
        }else if(c<0x800){//2个byte的utf-8
            putchar(0xc0|(c>>6));
            putchar(0x80|(c&0x3f));
        }else{//3个byte的utf-8
            putchar(0xe0|((c>>12)&0x0f));
            putchar(0x80|((c>>6)&0x3f));
            putchar(0x80|(c&0x3f));
        }
        //printf("(%d)",result[i]);

        if((label_info[result[i]][0]=='2')||(label_info[result[i]][0]=='3')){//分词位置
            if(*(label_info[result[i]]+1))//输出标签（如果有的话）
                printf("%s",label_info[result[i]]+1);
            if((i+1)<len)putchar(' ');//在分词位置输出空格
        }
    }
}

inline void add_values(int *value_offset,int base,int del,int* p_allowed_label=NULL){
    int ind=bases[base]+del;
    if(ind>=dat_size||checks[ind]!=base){
        return;
    }
    int offset=bases[bases[base]+del];
    int* weight_offset=model->fl_weights+offset*model->l_size;
    //p_allowed_label=NULL;
    int allowed_label;
    if(model->l_size==4){
        value_offset[0]+=weight_offset[0];
        value_offset[1]+=weight_offset[1];
        value_offset[2]+=weight_offset[2];
        value_offset[3]+=weight_offset[3];
    }else{
        if(p_allowed_label){
            while((allowed_label=(*(p_allowed_label++)))>=0){
                value_offset[allowed_label]+=weight_offset[allowed_label];
            }
        }else{
            for(int j=0;j<model->l_size;j++){
                value_offset[j]+=weight_offset[j];
            }
        }
    }
}
void put_values(){
    /*nodes*/
    for(int i=0;i<len;i++){
        nodes[i].type=0;
    }
    nodes[0].type+=1;
    nodes[len-1].type+=2;
    
    ///allowed_label_lists
    if(allowed_labels){
        
        for(int i=0;i<len;i++){
            if(char_label_map[sequence[i]]==-1){
                allowed_label_lists[i]=NULL;
            }else{
                allowed_label_lists[i]=allowed_labels+char_label_map[sequence[i]];
            }
        }
    }
    /*values*/
    for(int i=0;i<len*model->l_size;i++){
        values[i]=0;
    }
    find_bases(bases,checks,dat_size,35,35,uni_bases[0],bi_bases[0]);
    find_bases(bases,checks,dat_size,35,sequence[0],uni_bases[0],bi_bases[1]);
    for(int i=0;i+1<len;i++)
        find_bases(bases,checks,dat_size,sequence[i],sequence[i+1],uni_bases[i+1],bi_bases[i+2]);
    find_bases(bases,checks,dat_size,sequence[len-1],35,uni_bases[len],bi_bases[len+1]);
    find_bases(bases,checks,dat_size,35,35,uni_bases[len+1],bi_bases[len+2]);
    
    int base=0;
    for(int i=0;i<len;i++){
        int* value_offset=values+i*model->l_size;
        if((base=uni_bases[i+1])!=-1)
            add_values(value_offset,base,49,allowed_label_lists[i]);
        if((base=uni_bases[i])!=-1)
            add_values(value_offset,base,50,allowed_label_lists[i]);
        if((base=uni_bases[i+2])!=-1)
            add_values(value_offset,base,51,allowed_label_lists[i]);
        if((base=bi_bases[i+1])!=-1)
            add_values(value_offset,base,49,allowed_label_lists[i]);
        if((base=bi_bases[i+2])!=-1)
            add_values(value_offset,base,50,allowed_label_lists[i]);
        if((base=bi_bases[i])!=-1)
            add_values(value_offset,base,51,allowed_label_lists[i]);
        if((base=bi_bases[i+3])!=-1)
            add_values(value_offset,base,52,allowed_label_lists[i]);
    }
    
}
#endif
