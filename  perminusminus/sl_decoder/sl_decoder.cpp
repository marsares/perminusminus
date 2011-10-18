#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "sl_decoder.h"

int sl_decoder_show_sentence=0;



void output_allow_tagging(){
    //threshold=15000;
    /*找到score最大值*/
    int max_score=alphas[0].value+betas[0].value-values[0];
    int max_ind=0;
    for(int i=1;i<model->l_size;i++){
        int score=alphas[i].value+betas[i].value-values[i];
        if(score>max_score){
            max_score=score;
            max_ind=i;
        }
    }
    //printf("max=%d\n",max_score);
    //printf("\n");
    /*找出可能的标注序*/
    for(int i=0;i<len*model->l_size;i++)
        is_good_choice[i]=alphas[i].value+betas[i].value-values[i]+threshold>=max_score;
    
    /*找出可能的词*/
    int this_score=0;
    int left_part=0;
    int last_id=0;
    for(int i=0;i<len;i++){
        //printf("i=%d\n",i);
        for(int b_label_i=0;b_label_i<model->l_size;b_label_i++){
            if(!is_good_choice[i*model->l_size+b_label_i]){
                continue;
            }
            if(label_info[b_label_i][0]=='3'){
                //输出单个字的词
                this_score=alphas[i*model->l_size+b_label_i].value
                        +betas[i*model->l_size+b_label_i].value
                        -values[i*model->l_size+b_label_i];
                printf("%d,%d,%s,%d ",i,i+1,label_info[b_label_i]+1,max_score-this_score);
            }else if(label_info[b_label_i][0]=='0'){
                int mid_ind=label_looking_for[b_label_i][0];
                int right_ind=label_looking_for[b_label_i][1];
                left_part=alphas[i*model->l_size+b_label_i].value;
                last_id=b_label_i;
                for(int j=i+1;j<len;j++){
                    if(j==len)break;
                    if(right_ind==-1)break;

                    if(is_good_choice[j*model->l_size+right_ind]){
                        //check，是不是合格的词
                        this_score=left_part
                                +model->ll_weights[last_id*model->l_size+right_ind]
                                +betas[j*model->l_size+right_ind].value;
                        if(max_score-this_score<=threshold)
                            printf("%d,%d,%s,%d ",i,j+1,label_info[b_label_i]+1,max_score-this_score);
                    }
                    if(mid_ind==-1)break;
                    if(!is_good_choice[(j*(model->l_size))+mid_ind])
                        break;
                    left_part+=values[j*model->l_size+mid_ind]
                            +model->ll_weights[last_id*model->l_size+mid_ind];
                    last_id=mid_ind;
                }
                
            }
        }
    }
    //printf("end of a line \n");
}


/*对缓存里的串分词并编码成utf-8输出*/
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
        output_allow_tagging();
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
                sequence[len++]=c;//+65248;//半角转全角，放入缓存
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
}

int main (int argc,char **argv) {
    threshold=0;
    
    int c;
    char* label_trans=NULL;
    char* label_lists_file=NULL;
    while ( (c = getopt(argc, argv, "b:u:t:sh")) != -1) {
        switch (c) {
            case 'b' : ///label binary的约束
                label_trans = optarg;
                break;
            case 'u' : ///label unary的约束
                label_lists_file = optarg;
                break;
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
    
    init(argv[optind],argv[optind+1],argv[optind+2],label_trans,label_lists_file);

    read_stream();
    return 0;
}
