#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "decoder.h"
#include "model.h"

/*缓存大小*/
const int max_length=10000;

/*句子*/
int len=0;//句子长度
int sequence[max_length];//句子

/*特征、双数组相关*/
int uni_bases[max_length+2];
int bi_bases[max_length+4];

/*双数组*/
int dat_size;//双数组大小
int* bases;//base数组
int* checks;//check数组

/*模型*/
Model* model=new Model("model.bin");

/*解码用*/
Node nodes[max_length];//只用来存储解码用的节点的拓扑结构
int* values=(int*)calloc(4,max_length*model->l_size);
Alpha_Beta* alphas=(Alpha_Beta*)calloc(sizeof(Alpha_Beta),max_length*model->l_size);
int* result=(int*)calloc(4,max_length*model->l_size);

/*初始化一个双数组trie树*/
void load_da(char* filename,int* &base_array,int* &check_array,int &size){
    //打开文件
    FILE * pFile=fopen ( filename , "rb" );
    /*得到文件大小*/
    fseek (pFile , 0 , SEEK_END);
    size=ftell (pFile)/4/2;//双数组大小
    rewind (pFile);//重置文件指针到开头
    /*前一半是base数组，后一半是check数组*/
    base_array=(int*) malloc (sizeof(int)*size);
    check_array=(int*) malloc (sizeof(int)*size);
    fread (base_array,4,size,pFile);
    fread (check_array,4,size,pFile);
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

void init(){
    for(int i=0;i<max_length;i++){
        int* pr=new int[2];
        pr[0]=i-1;
        pr[1]=-1;
        nodes[i].predecessors=pr;
    };
    load_da("dat.bin",bases,checks,dat_size);
}

void dp(){
    dp_decode(
            model->l_size,//check
            model->ll_weights,//check
            len,//check
            nodes,
            values,
            alphas,
            result
        );
}

inline void add_values(int *value_offset,int base,int del){
    int ind=bases[base]+del;
    if(ind>=dat_size||checks[ind]!=base){
        return;
    }
    int offset=bases[bases[base]+del];
    int* weight_offset=model->fl_weights+offset*4;
    value_offset[0]+=weight_offset[0];
    value_offset[1]+=weight_offset[1];
    value_offset[2]+=weight_offset[2];
    value_offset[3]+=weight_offset[3];
}
void put_values(){
    /*nodes*/
    for(int i=0;i<len;i++){
        nodes[i].type=0;
    }
    nodes[0].type+=1;
    nodes[len-1].type+=2;
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
        int* value_offset=values+i*4;
        if((base=uni_bases[i+1])!=-1)
            add_values(value_offset,base,49);
        if((base=uni_bases[i])!=-1)
            add_values(value_offset,base,50);
        if((base=uni_bases[i+2])!=-1)
            add_values(value_offset,base,51);
        if((base=bi_bases[i+1])!=-1)
            add_values(value_offset,base,49);
        if((base=bi_bases[i+2])!=-1)
            add_values(value_offset,base,50);
        if((base=bi_bases[i])!=-1)
            add_values(value_offset,base,51);
        if((base=bi_bases[i+3])!=-1)
            add_values(value_offset,base,52);
    }
    
}
/*对缓存里的串分词并编码成utf-8输出*/
void output(){
    put_values();//检索出特征值并初始化放在values数组里
    dp();//动态规划搜索最优解放在result数组里
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
        //putchar(48+result[i]);
        //在分词位置输出空格
        if((result[i]>1)&&((i+1)<len)
           )
            putchar(' ');
    }
}

int main () {
    init();
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
                sequence[len++]=c+65248;//半角转全角，放入缓存
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
    
    return 0;
}
