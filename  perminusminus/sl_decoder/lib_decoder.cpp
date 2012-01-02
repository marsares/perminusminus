#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "lib_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

//using namespace std;

daidai::TaggingDecoder* tagging_decoder;


int punks[65535];
int unicode_input[1000];
int unicode_offsets[1000];
int unicode_tags[1000];
int* gbk_unicode_table;
//int offsets[1000];
//int lengths[1000];


int load_gbk_to_unicode(){
    FILE* pFile;
    int rtn_value;
    pFile=fopen("data/gbk_to_unicode.table","rb");
    //printf("%d,",pFile);
    
    gbk_unicode_table=(int*)malloc(sizeof(int)*256*256);
    //printf("%d\n",gbk_unicode_table[44444]);
    
    rtn_value=fread(gbk_unicode_table,4,256*256,pFile);
    //printf("%d,\n",rtn_value);
    //printf("%d\n",gbk_unicode_table[44444]);
    fclose(pFile);
}


int test(){
    printf("haha\n");
    int*input=new int[1000];
    int*tags=new int[1000];
    int max_length=100;
    int length=0;
    int rtn=1;
    
    length=3;
    input[0]=55;
    input[1]=55;
    input[2]=55;
    tagging_decoder->segment(input,length,tags);
    printf("%d %d %d",tags[0],tags[1],tags[2]);
}

int cws_segment(int* intput,int length,int* tags){
    tagging_decoder->segment(intput,length,tags);
}

int gbk_to_unicode(char* input,int length,int &char_length){
    int c=0;
    char_length=0;
    int offset=0;
    for(int i=0;i<length;i++){
        c=c*256+(unsigned char)input[i];
        //printf("%d %d \n",i,c);
        if(c>=256){
            unicode_offsets[char_length]=offset;
            offset=i+1;
            unicode_input[char_length++]=gbk_unicode_table[c];
            if(unicode_input[char_length]==-1)unicode_input[char_length]='?';
            c=0;
        }else if(c<128){
            unicode_offsets[char_length]=offset;
            offset=i+1;
            unicode_input[char_length++]=gbk_unicode_table[c]+65248;
            if(unicode_input[char_length]==-1)unicode_input[char_length]='?';
            c=0;
        }else{
            
        }
    }
    unicode_offsets[char_length]=length;
}

int is_punc(int begin,int end){
    for(int i=begin;i<end;i++){
        if(unicode_input[i]<65535&&punks[unicode_input[i]])return true;
    }
    return false;
};

int cws_gbk_2(char* input,int length,int*offsets,int*lengths,int show_punk=true){
    int char_length=0;
    int tags[1000];
    gbk_to_unicode(input,length,char_length);
    
    //printf("%d\n",char_length);
    //tagging_decoder->label_trans_pre=NULL;
    //tagging_decoder->allowed_label_lists=NULL;
    tagging_decoder->segment(unicode_input,char_length,tags);
    

    int last=-1;
    int word_id=0;
    

    //offsets[0]=1;
    //return 0;
    for(int i=0;i<char_length;i++){
        if(tags[i]==1){
            
            if(last!=-1){
                if(show_punk||(!is_punc(last,i))){
                    //printf(":: %d %d\n",last,i);
                    offsets[word_id]=unicode_offsets[last];
                    lengths[word_id]=unicode_offsets[i]-unicode_offsets[last];
                    word_id++;
                }
            }
            last=i;
        }
    }
    //return 0;
    offsets[word_id]=unicode_offsets[last];
    lengths[word_id]=unicode_offsets[char_length]-unicode_offsets[last];
    word_id++;
    offsets[word_id]=-1;
    lengths[word_id]=-1;
    //p_offset=offsets;
    //p_length=p_length;
}

int cws_gbk(char* input,int length,int* tags,int &char_length){
    gbk_to_unicode(input,length,char_length);
    tagging_decoder->segment(unicode_input,char_length,tags);
}

int cws_init(){
    tagging_decoder=new daidai::TaggingDecoder();
    char model_file[]="data/model.bin";
    char dat_file[]="data/dat.bin";
    char label_file[]="data/label.txt";
    
    tagging_decoder->init(model_file+0,dat_file+0,label_file+0);
    
    
    
    load_gbk_to_unicode();
    for(int i=0;i<65535;i++){
        punks[i]=0;
    }
    punks[8221]=1;
    punks[8220]=1;
    punks[8216]=1;
    punks[8217]=1;
    punks[65307]=1;
    punks[65306]=1;
    punks[8230]=1;
    punks[8212]=1;
    punks[65292]=1;
    punks[65294]=1;
    punks[65306]=1;
    punks[65307]=1;
    punks[65311]=1;
    punks[65281]=1;
    punks[65282]=1;
    punks[65287]=1;

    
}

int cws_release(){
    delete tagging_decoder;
    free(gbk_unicode_table);
}


#ifdef __cplusplus
}
#endif
