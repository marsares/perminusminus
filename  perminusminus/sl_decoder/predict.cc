#include <cstdio>
#include <cstdlib>

#include <unistd.h>
#include <getopt.h>
#include "tagging_decoder.h"




using namespace daidai;



int main (int argc,char **argv) {
    
    int show_sentence=false;
    
    FILE* input_file=stdin;
    FILE* output_file=stdin;
    int threshold=0;
    static struct option long_options[] =
		{
			//{"verbose", no_argument,       &verbose_flag, 1},
			//{"seg_only",   no_argument,       &seg_only, 1},
			//{"help",     no_argument,       0, 'h'},
            {"threshold",     required_argument,       0, 't'},
			{"show_input",  no_argument,       0, 'T'},
            {"input",  required_argument,       0, 'i'},
            {"output",  required_argument,       0, 'o'},
            
			{0, 0, 0, 0}
		};
    int c;
    int option_index = 0;
    while ( (c = getopt_long(argc, argv, "o:i:Tt:h",long_options,&option_index)) != -1) {
        switch (c) {
            case 0:
                break;
            case 'T':
                show_sentence=true;
                break;
            case 'i':
                input_file=fopen(optarg,"r");
                break;
            case 'o':
                output_file=fopen(optarg,"r");
                break;
            case 't' : 
                threshold=atoi(optarg);
                break;
            case 'h' :
            case '?' : 
            default :
                fprintf(stderr,"");
                return 1;
        }
    }
    
    
    
    
    if(!(optind<argc)){
        fprintf(stderr,"need one augument for prefix for model files\n");
        return 1;
    }

    std::string model_filename_prefix(argv[optind]);
    
    TaggingDecoder* tagging_decoder=new TaggingDecoder();
    tagging_decoder->threshold=threshold*1000;
    tagging_decoder->init(
            (model_filename_prefix+"_model.bin").c_str(),
            (model_filename_prefix+"_dat.bin").c_str(),
            (model_filename_prefix+"_label.txt").c_str());
    tagging_decoder->set_label_trans();
    
    int*input=new int[10000];
    int*tags=new int[10000];
    int max_length=10000;
    int length=0;
    int rtn=1;
    while(1){
        rtn=daidai::get_raw(input,max_length,length,input_file);
        if(length){
            tagging_decoder->segment(input,length,tags);
            if(tagging_decoder->threshold==0){
                tagging_decoder->output_sentence();
            }else{
                if(show_sentence){
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
    
    delete tagging_decoder;
    
    if(input_file!=stdin)fclose(input_file);
    if(output_file!=stdin)fclose(output_file);
    
    return 0;
}


