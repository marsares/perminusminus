#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include "scripts.h"

using namespace permm;

void showhelp(){
    printf("per-- toolkit\n");
    printf("author: ZHANG Kaixu\n");
    printf("see http://code.google.com/p/perminusminus/ for more info.\n");
    return;
}

int main(int argc, char **argv){
    //only for development
    //train("training.bin","model.bin",5);
    //test("model.bin","test.bin","result.txt",1);
    //end of development code
    
    
    if(argc==1){
        showhelp();
        return 0;
    }
    
    int c;
    char* allowed_label_bigram_file=NULL;
    int output_config=0;
    //char* label_lists_file=NULL;
    while ( (c = getopt(argc, argv, "12b:h")) != -1) {
        switch (c) {
            case '1' : 
                output_config=-1;
                break;
            case '2' : 
                output_config=-2;
                break;
            case 'b' : 
                allowed_label_bigram_file=optarg;
                break;
            case 'u' : 
                //label_lists_file = optarg;
                break;
            case 'h' :
            case '?' : 
            default : 
                showhelp();
                return 1;
        }
    }
    
    
    char* strend;
    if(argv[optind][0]=='l'){
        train(argv[optind+1],
                argv[optind+2],
                strtol(argv[optind+3],&strend,10),allowed_label_bigram_file);
    }else if(argv[optind][0]=='p'){
        if(optind+4<argc){
            output_config=strtol(argv[optind+4],&strend,10);
        }
        test(argv[optind+1],
             argv[optind+2],
             argv[optind+3],
             output_config,
             allowed_label_bigram_file);
        
    }
    
    return 0;

}



