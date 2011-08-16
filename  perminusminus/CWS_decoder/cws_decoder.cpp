#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "decoder.h"
#include "model.h"

/*�����С����ע����С*/
const int max_length=10000;


int sequence[max_length];
int uni_bases[max_length+2];
int bi_bases[max_length+4];
Node nodes[max_length];



int len=0;

int dat_size;
int* bases;
int* checks;
/* */

Model* model=new Model("model.bin");
int* values=(int*)calloc(4,max_length*model->l_size);
Alpha_Beta* alphas=(Alpha_Beta*)calloc(sizeof(Alpha_Beta),max_length*model->l_size);
int* result=(int*)calloc(4,max_length*model->l_size);

/*��ʼ��һ��˫����trie��*/
void load_da(char* filename,int* &base_array,int* &check_array,int &size){
    //���ļ�
    FILE * pFile=fopen ( filename , "rb" );
    /*�õ��ļ���С*/
    fseek (pFile , 0 , SEEK_END);
    size=ftell (pFile)/4/2;//˫�����С
    rewind (pFile);//�����ļ�ָ�뵽��ͷ
    /*ǰһ����base���飬��һ����check����*/
    base_array=(int*) malloc (sizeof(int)*size);
    check_array=(int*) malloc (sizeof(int)*size);
    fread (base_array,4,size,pFile);
    fread (check_array,4,size,pFile);
    //�ر��ļ�
    fclose (pFile);
};
/*
inline void find_bases(int*bases,int*checks,int dat_size,int ch1,int ch2,int& uni_base,int&bi_base){
    int ind=0;
    int ind2=0;
    int base=0;
    ind=bases[ind]+ch1;
    if(ind>=dat_size||checks[ind]!=base){
        uni_base=-1;bi_base=-1;return;
    }
    
    ind=bases[ind]+32;
    base=ind;
    ind=bases[ind]+49;
    base=ind;
    
    
    ind=bases[ind]+ch2;
    if(ind>=dat_size||checks[ind]!=base)return -1;
    base=ind;
    ind=bases[ind]+32;
    if(ind>=dat_size||checks[ind]!=base)return -1;
    base=ind;
    ind=bases[ind]+49;
    base=ind;
    
}*/

inline int find_uni_base(int*bases,int*checks,int dat_size,int ch1){
    int ind=0;
    ind=bases[ind]+ch1;
    if(ind>=dat_size||checks[ind])return -1;
    ind=bases[ind]+32;
    ind=bases[ind]+49;
    return ind;
}

inline int find_bi_base(int*bases,int*checks,int dat_size,int ch1,int ch2){
    int ind=0;
    ind=bases[ind]+ch1;
    if(ind>=dat_size||checks[ind])return -1;
    int base=ind;
    ind=bases[ind]+ch2;
    if(ind>=dat_size||checks[ind]!=base)return -1;
    ind=bases[ind]+32;
    ind=bases[ind]+49;
    return ind;
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
void put_values(){
    for(int i=0;i<len*model->l_size;i++){
        values[i]=0;
    }
    for(int i=0;i<len;i++){
        nodes[i].type=0;
    }
    nodes[0].type+=1;
    nodes[len-1].type+=2;
    //printf("%d\n",len);
    
    /*uni_bases*/
    uni_bases[0]=find_uni_base(bases,checks,dat_size,35);
    for(int i=0;i<len;i++){
        uni_bases[i+1]=find_uni_base(bases,checks,dat_size,sequence[i]);
    }
    uni_bases[len+1]=find_uni_base(bases,checks,dat_size,35);
    /*bi_bases*/
    bi_bases[0]=find_bi_base(bases,checks,dat_size,35,35);
    bi_bases[1]=find_bi_base(bases,checks,dat_size,35,sequence[0]);
    for(int i=0;i+1<len;i++){
        bi_bases[i+2]=find_bi_base(bases,checks,dat_size,sequence[i],sequence[i+1]);
    }
    bi_bases[len+2]=find_bi_base(bases,checks,dat_size,sequence[len-1],35);
    bi_bases[len+3]=find_bi_base(bases,checks,dat_size,35,35);
    
    int base=0;
    int left=0;int left2=0;
    int right=0;int right2=0;
    for(int i=0;i<len;i++){
        int* value_offset=values+i*4;
        int* weight_offset;
        base=uni_bases[i+1];
        if(base!=-1){
            int offset=bases[base];
            weight_offset=model->fl_weights+offset*4;
            for(int j=0;j<4;j++)
                value_offset[j]+=weight_offset[j];
        }
        base=uni_bases[i];
        if(base!=-1){
            int offset=bases[base+1];
            weight_offset=model->fl_weights+offset*4;
            for(int j=0;j<4;j++)
                value_offset[j]+=weight_offset[j];
        }
        base=uni_bases[i+2];
        if(base!=-1){
            int offset=bases[base+2];
            weight_offset=model->fl_weights+offset*4;
            for(int j=0;j<4;j++)
                value_offset[j]+=weight_offset[j];
        }
        
        base=bi_bases[i+1];
        if(base!=-1){
            int offset=bases[base];
            weight_offset=model->fl_weights+offset*4;
            for(int j=0;j<4;j++)
                value_offset[j]+=weight_offset[j];
        }
        base=bi_bases[i+2];
        if(base!=-1){
            int offset=bases[base+1];
            weight_offset=model->fl_weights+offset*4;
            for(int j=0;j<4;j++)
                value_offset[j]+=weight_offset[j];
        }
        base=bi_bases[i];
        if(base!=-1){
            int offset=bases[base+2];
            weight_offset=model->fl_weights+offset*4;
            for(int j=0;j<4;j++)
                value_offset[j]+=weight_offset[j];
        }
        base=bi_bases[i+3];
        if(base!=-1){
            int offset=bases[base+3];
            weight_offset=model->fl_weights+offset*4;
            for(int j=0;j<4;j++)
                value_offset[j]+=weight_offset[j];
        }
    }
    
}
/*�Ի�����Ĵ��ִʲ������utf-8���*/
void output(){
    put_values();//����������ֵ����ʼ������values������
    dp();//��̬�滮�������Ž����result������
    int c;
    for(int i=0;i<len;i++){
        c=sequence[i];
        if(c<128){//1��byte��utf-8
            putchar(c);
        }else if(c<0x800){//2��byte��utf-8
            putchar(0xc0|(c>>6));
            putchar(0x80|(c&0x3f));
        }else{//3��byte��utf-8
            putchar(0xe0|((c>>12)&0x0f));
            putchar(0x80|((c>>6)&0x3f));
            putchar(0x80|(c&0x3f));
        }
        //putchar(48+result[i]);
        //�ڷִ�λ������ո�
        if((result[i]>1)&&((i+1)<len)
           )
            putchar(' ');
    }
}

int main () {
    init();
    /*
     ��������ѭ��
     �������ͣ�Ķ���utf-8������ַ�������������ʽ���롣
     �����һ���ַ������뻺��ȴ��ִʡ�
     ����ǷǴ�ӡ�ַ�����س������߿ո���Ի����е��ַ����ִ������
        ����������ַ���
    ֱ���ļ�ĩβ������
    */
    int c;
    while(1){//������ȡ������ֱ���ļ�ĩβ
        c=getchar();
        if(c==EOF){
            if(len)output();
            fflush(stdout);
            break;
        }
        if(!(c&0x80)){//1��byte��utf-8����
            if(c<=32){//�Ǵ�ӡ�ַ����ո�
                if(len)output();//�Ի����еĴ��ִʲ����
                len=0;//��ջ���
                putchar(c);
                fflush(stdout);//flush���������ʱ����ʾ��ȥ��
            }else{//һ��ascii�ַ�
                sequence[len++]=c+65248;//���תȫ�ǣ����뻺��
            }
        }else if(!(c&0x20)){//2��byte��utf-8����
            sequence[len++]=((c&0x1f)<<6)|
                (getchar()&0x3f);
        }else if(!(c&0x10)){//3��byte��utf-8����
            sequence[len++]=((c&0x0f)<<12)|
                ((getchar()&0x3f)<<6)|
                (getchar()&0x3f);
        }else{//�����unicode���벻�ܴ���
            break;
        }
    }
    
    return 0;
}
