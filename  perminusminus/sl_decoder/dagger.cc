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
    TaggingDecoder* tag_decoder=new TaggingDecoder();
    tag_decoder->threshold=0;
    cws_decoder->threshold=15000;
    
    int c;
    while ( (c = getopt(argc, argv, "t:h")) != -1) {
        switch (c) {
            case 't' : //specify the threshold for the CWS model
                cws_decoder->threshold = atoi(optarg)*1000;
                break;
            case 'h' :
            case '?' : 
            default : 
                showhelp();
                return 1;
        }
    }
    ///cws解码器初始化
    std::string seg_prefix(argv[optind]);
    std::string tag_prefix(argv[optind+1]);

    //char* seg_model=argv[optind];
    //char* seg_dat=argv[optind+1];
    //char* seg_label=argv[optind+2];
    int seg_label_ind[]={0,0,0,0};
    FILE *fp;
    fp = fopen((seg_prefix+"_label.txt").c_str(), "r");
    int ind=0;
    int value=0;
    while( fscanf(fp, "%d", &value)==1){
        seg_label_ind[ind++]=value;
    }
    fclose(fp);
    //cws_decoder->init(seg_model,seg_dat,seg_label);
    cws_decoder->init((seg_prefix+"_model.bin").c_str()
            ,(seg_prefix+"_dat.bin").c_str()
            ,(seg_prefix+"_label.txt").c_str());
    cws_decoder->set_label_trans();

    
    ///tagging的解码器初始化
    
    
    //char* tag_model=argv[optind+3];
    //char* tag_dat=argv[optind+4];
    //char* tag_label=argv[optind+5];
    
   // tag_decoder->is_old_type_dat=true;
    tag_decoder->init((tag_prefix+"_model.bin").c_str()
            ,(tag_prefix+"_dat.bin").c_str()
            ,(tag_prefix+"_label.txt").c_str());
    //tag_decoder->init(tag_model,tag_dat,tag_label);
    tag_decoder->set_label_trans();
    
    int l_size=tag_decoder->model->l_size;
    
    std::list<int> allowed_tags_lists[16];
    

    fp = fopen((tag_prefix+"_label.txt").c_str(), "r");
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
    int max_length=1000;
    int length=0;
    int rtn=1;
    while(rtn){
        rtn=daidai::get_raw(input,max_length,length);
        if(length){
            cws_decoder->segment(input,length,tags);
            cws_decoder->cal_betas();
            cws_decoder->find_good_choice();
            
            int seg_ind;
            for(int i=0;i<length;i++){
                int tag_code=0;
                for(int j=0;j<4;j++){
                    seg_ind=seg_label_ind[j];
                    tag_code+=(cws_decoder->is_good_choice[i*4+j]<<seg_ind);
                }
                tag_decoder->allowed_label_lists[i]=allowed_tags[tag_code];
            }
            tag_decoder->segment(input,length,tags);
            tag_decoder->output_sentence();
        }
        if(rtn==-1)break;
        putchar(rtn);
    }
    
    delete[] input;
    delete[] tags;
    for(int i=1;i<16;i++)delete[] allowed_tags[i];

    delete cws_decoder;
    delete tag_decoder;

    return 0;
}


#ifdef __cplusplus
}
#endif
