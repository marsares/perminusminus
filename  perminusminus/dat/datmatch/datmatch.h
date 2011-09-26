#ifndef __DATMATCH_H__
#define __DATMATCH_H__
#include <stdio.h>
#include <stdlib.h>
/*初始化一个双数组trie树*/
void dat_load(char* filename,int* &base_array,int* &check_array,int &size){
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
int dat_print_match(int* bases,int*checks,int dat_size,int* word,int length){
    int empty=1;
    for(int offset=0;offset<length;offset++){
        int pre_base=0;
        int pre_ind=0;
        int ind=0;
        //printf("%d\n",offset);
        for(int i=offset;i<length;i++){
            ind=pre_base+word[i];
            if(ind<0||ind>=dat_size||checks[ind]!=pre_ind)break;
            pre_ind=ind;
            pre_base=bases[ind];
            ind=pre_base+32;
            if(!(ind<0||ind>=dat_size||checks[ind]!=pre_ind)){
                if(empty){
                    printf("%d:%d",offset,i+1);
                    empty=0;
                }else{
                    printf(" %d:%d",offset,i+1);
                }
            }
        }
    }
    return !empty;
}
void dat_find_prefix(int* bases,int*checks,int dat_size,int* word,int length){
    int pre_base=0;
    int pre_ind=0;
    int ind=0;
    for(int i=0;i<length;i++){
        printf("input %d\n",word[i]);
        ind=pre_base+word[i];
        if(ind<0||ind>=dat_size||checks[ind]!=pre_ind)return;
        pre_ind=ind;
        pre_base=bases[ind];
        ind=pre_base+32;
        if(!(ind<0||ind>=dat_size||checks[ind]!=pre_ind)){
            printf("%d\n",i);
        }
    }
}

#endif
