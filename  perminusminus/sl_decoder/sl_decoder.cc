#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "tagging_decoder.h"
#include "daidai_base.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace daidai;

void showhelp(){
}

int main (int argc,char **argv) {
    TaggingDecoder* tagging_decoder=new TaggingDecoder();
    
    int sl_decoder_show_sentence=0;
    
    tagging_decoder->threshold=0;
    
    int c;
    char* label_trans=NULL;
    int is_old_type_dat=false;
    while ( (c = getopt(argc, argv, "b:t:shD")) != -1) {
        switch (c) {
            case 'D' : ///old type的DAT树
                is_old_type_dat=true;
            case 'b' : ///label binary的约束
                label_trans = optarg;
                break;
            case 't' : ///output word lattice
                tagging_decoder->threshold = atoi(optarg)*1000;
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
    
    tagging_decoder->is_old_type_dat=is_old_type_dat;
    tagging_decoder->init(argv[optind],argv[optind+1],argv[optind+2],label_trans);
    if(!label_trans)tagging_decoder->set_label_trans();
    //read_stream();
    int*input=new int[1000];
    int*tags=new int[1000];
    int max_length=1000;
    int length=0;
    int rtn=1;
    daidai::RawSentence raw;
    daidai::SegmentedSentence segged;
    while(1){
        rtn=daidai::get_raw(input,max_length,length);
        if(length){
            tagging_decoder->segment(input,length,tags);
            if(tagging_decoder->threshold==0){
                tagging_decoder->output_sentence();
            }else{
                if(sl_decoder_show_sentence){
                    tagging_decoder->output_raw_sentence();
                    printf(" ");
                }
                tagging_decoder->cal_betas();
                tagging_decoder->output_allow_tagging();
            }
        }
        if(rtn==-1)break;
        putchar(rtn);
    }
    delete[]input;
    delete[]tags;
    delete[]label_trans;
    delete tagging_decoder;
    return 0;
}


#ifdef __cplusplus
}
#endif
