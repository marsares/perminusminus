#include <cstdio>
#include <cstdlib>
#include <list>
#include <unistd.h>
#include "tagging_decoder.h"

using namespace daidai;

#ifdef __cplusplus
extern "C" {
#endif

void showhelp(){
}

int main (int argc,char **argv) {
    TaggingDecoder* cws_decoder=new TaggingDecoder();
    int sl_decoder_show_sentence=0;
    TaggingDecoder* tag_decoder=new TaggingDecoder();
    tag_decoder->threshold=0;
    cws_decoder->threshold=15000;
    
    int c;
    char* label_trans=NULL;
    char* label_lists_file=NULL;
    while ( (c = getopt(argc, argv, "t:sh")) != -1) {
        switch (c) {
            case 't' : ///output word lattice
                cws_decoder->threshold = atoi(optarg)*1000;
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
    ///cws解码器初始化
    char* seg_model=argv[optind];
    char* seg_dat=argv[optind+1];
    char* seg_label=argv[optind+2];
    int seg_label_ind[]={0,0,0,0};
    FILE *fp;
    fp = fopen(seg_label, "r");
    int ind=0;
    int value=0;
    while( fscanf(fp, "%d", &value)==1){
        seg_label_ind[ind++]=value;
    }
    fclose(fp);
    cws_decoder->init(seg_model,seg_dat,seg_label,NULL,NULL);

    
    ///tagging的解码器初始化
    
    
    char* tag_model=argv[optind+3];
    char* tag_dat=argv[optind+4];
    char* tag_label=argv[optind+5];
    
    tag_decoder->init(tag_model,tag_dat,tag_label,NULL,NULL);
    tag_decoder->set_label_trans();
    
    int l_size=tag_decoder->model->l_size;
    
    std::list<int> allowed_tags_lists[16];
    

    fp = fopen(tag_label, "r");
    char char_cache[16];
    ind=0;
    
    while( fscanf(fp, "%s", char_cache)==1){
        int seg_ind=char_cache[0]-'0';
        for(int j=0;j<16;j++){
            if((1<<seg_ind)&(j)){
                allowed_tags_lists[j].push_back(ind);
            }
        }
        //printf("%d ",char_cache[0]-'0');
        ind++;
    }
    fclose(fp);
    
    int* allowed_tags[16];
    for(int j=1;j<16;j++){
        allowed_tags[j]=new int[(int)allowed_tags_lists[j].size()+1];
        int k=0;
        for(std::list<int>::iterator plist = allowed_tags_lists[j].begin();
                plist != allowed_tags_lists[j].end(); plist++){
            allowed_tags[j][k++]=*plist;
        };
        allowed_tags[j][k]=-1;
    }
    
    //printf("(l_size = %d)\n",l_size);
    
    
    
    
    
    //read_stream();
    ///开始运行
    int*input=new int[1000];
    int*tags=new int[1000];
    int max_length=100;
    int length=0;
    int rtn=1;
    while(rtn){

        rtn=cws_decoder->get_input_from_stream(input,max_length,length);
        if(!rtn)break;
        cws_decoder->segment(input,length,tags);
        
        
        //cws_decoder->output_sentence();
        //printf("\n");
        
        cws_decoder->cal_betas();
        //cws_decoder->output_allow_tagging();
        //printf("\n");
        cws_decoder->find_good_choice();
        
        
        int seg_ind;
        for(int i=0;i<length;i++){
            int tag_code=0;
            for(int j=0;j<4;j++){
                seg_ind=seg_label_ind[j];
                //printf("%d,%d ",seg_ind,cws_decoder->is_good_choice[i*4+j]);
                tag_code+=(cws_decoder->is_good_choice[i*4+j]<<seg_ind);
            }
            //printf(" %d\n",(int)allowed_tags_lists[tag_code].size());
            tag_decoder->allowed_label_lists[i]=allowed_tags[tag_code];
        }
        
        tag_decoder->segment(input,length,tags);
        tag_decoder->output_sentence();

        printf("\n");
    }
    return 0;
}


#ifdef __cplusplus
}
#endif
