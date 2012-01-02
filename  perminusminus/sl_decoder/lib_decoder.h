#ifndef __LIB_DECODER_H__
#define __LIB_DECODER_H__
#include "tagging_decoder.h"
#ifdef __cplusplus
extern "C" {
#endif
//using namespace std;



int s=100;

/*
 * 初始化分词模块
 * */
int cws_init();
/*
 * 释放空间
 * */
int cws_release();

/*
 * gbk分词
 * 
 * char* input 待分词的gbk编码的汉字字符串头指针
 * int length 待分词的gbk编码的汉字字符长度（char长度，非汉字个数）
 * int* tags 用以存储分词字标注的int数组头指针
 * int &char_length tags数组长度
 * 
 * tags参数与char_length参数返回结构，tags的空间需要预先分配
 * 
 * char_length 长度是输入串包含汉字（包括其它标点符号）个数
 * tags[i]=1 表示第i个汉字是一个词的开头一个字
 * tags[i]=0 表示第i个汉字不是一个词的开头一个字
 * */
int cws_gbk(char* input,int length,int* tags,int &char_length);

/*
 * gbk分词2
 * 
 * char* input 待分词的gbk编码的汉字字符串头指针
 * int length 待分词的gbk编码的汉字字符长度（char长度，非汉字个数）
 * int*offsets 每个词偏移量的数组，当遇到-1时表示数组末尾
 * int*lengths 每个词长度的数组，当遇到-1时表示数组末尾
 * int show_punk 设置是否输出标点 1 是输出， 0是不输出
 * 
 * offsets数组和lengths数组空间需要预先分配
 * 
 * 分词结果第i个词，从input数组的下标offsets[i]开始，长度是offsets[i]个char。
 * */
int cws_gbk_2(char* input,int length,int*offsets,int*lengths,int show_punk);


#ifdef __cplusplus
}
#endif

#endif
