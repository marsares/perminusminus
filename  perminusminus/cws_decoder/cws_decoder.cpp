#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "sl_decoder.h"
int sl_decoder_show_sentence=0;

void output_allow_cws(){
    int max_ind=0;
    int max_score=alphas[0].value+betas[0].value-values[0];
    for(int i=1;i<4;i++){
        int score=alphas[i].value+betas[i].value-values[i];
        if(score>max_score){
            max_score=score;
            max_ind=i;
        }
    }
    
    for(int i=0;i<len;i++){
        int va=alphas[i*4].value+betas[i*4].value-values[i*4];
        int vb=alphas[i*4+1].value+betas[i*4+1].value-values[i*4+1];
        int score=va>vb?va:vb;
        allow_com[i]=(score+threshold>=max_score);
        va=alphas[i*4+2].value+betas[i*4+2].value-values[i*4+2];
        vb=alphas[i*4+3].value+betas[i*4+3].value-values[i*4+3];
        score=va>vb?va:vb;
        allow_sep[i]=(score+threshold>=max_score);
    }
    //allow_sep[len-1]=1;
    //printf("\n");
    int m_score=0;
    for(int i=0;i<len;i++){
        if((i>0)&&(!allow_sep[i-1]))continue;
        m_score=0;
        for(int j=i;j<len;j++){
            if(allow_sep[j]||j==(len-1)){
                if(i!=j){//多字词
                    int score=alphas[i*4].value+betas[j*4+2].value;
                    if(i+1==j){
                        score+=model->ll_weights[2];
                    }else{
                        score+=m_score+model->ll_weights[1]+model->ll_weights[6]
                                +model->ll_weights[5]*(j-i-2);
                    }
                    if(score+threshold>=max_score)printf("%d,%d,%d ",i,j+1,
                        max_score-score);
                }else{//单字词
                    //输出边界距离
                    printf("%d,%d,%d ",i,j+1,
                        max_score-(alphas[i*4+3].value+betas[i*4+3].value-values[i*4+3]));
                }
            }
            if(!allow_com[j])break;
            if(i!=j){
                m_score+=values[j*4+1];
            }
        }
    }
}
void output(){
    put_values();//检索出特征值并初始化放在values数组里
    dp();//动态规划搜索最优解放在result数组里
    
    if(threshold==0){
        output_sentence();
    }else{
        if(sl_decoder_show_sentence){
            output_raw_sentence();
            printf(" ");
        }
        cal_betas();
        output_allow_cws();
    }
}

void read_stream(){
    /*
     这里是主循环
     在这里，不停的读入utf-8编码的字符，根据其编码格式解码。
     如果是一般字符，放入缓存等待分词。
     如果是非打印字符（如回车）或者空格，则对缓存中的字符串分词输出，
        接着输出该字符。
    直到文件末尾结束。
    */
    int c;
    while(1){//反复读取输入流直到文件末尾
        c=getchar();
        if(c==EOF){
            if(len)output();
            fflush(stdout);
            break;
        }
        if(!(c&0x80)){//1个byte的utf-8编码
            if(c<=32){//非打印字符及空格
                if(len)output();//对缓存中的串分词并输出
                len=0;//清空缓存
                putchar(c);
                fflush(stdout);//flush输出流，即时的显示出去。
            }else{//一般ascii字符
                sequence[len++]=c;//+65248;//(半角转全角，放入缓存)
            }
        }else if(!(c&0x20)){//2个byte的utf-8编码
            sequence[len++]=((c&0x1f)<<6)|
                (getchar()&0x3f);
        }else if(!(c&0x10)){//3个byte的utf-8编码
            sequence[len++]=((c&0x0f)<<12)|
                ((getchar()&0x3f)<<6)|
                (getchar()&0x3f);
        }else{//更大的unicode编码不能处理
            break;
        }
    }
}

void showhelp(){
    printf("Chinese Word Segmenter\n");
    printf("----Author: Kaixu Zhang @ Tsinghua University\n");
    printf("arguments:\n");
    printf("  modelfile indexfile labelfile threshold\n");
}

int main (int argc,char **argv) {
    if(argc==1){
        showhelp();
        return 0;
    }
    
    threshold=0;
    int c;
    char* label_trans=NULL;
    char* label_lists_file=NULL;
    while ( (c = getopt(argc, argv, "t:sh")) != -1) {
        switch (c) {
            case 't' : ///output word lattice
                threshold = atoi(optarg)*1000;
                break;
            case 's':///输出lattice之前输出原句
                sl_decoder_show_sentence=1;
                break;
            case 'h' :
            case '?' : 
            default : 
                showhelp();
                return 1;
        }
    }
    init(argv[optind],argv[optind+1],argv[optind+2]);
    read_stream();
    return 0;
}
