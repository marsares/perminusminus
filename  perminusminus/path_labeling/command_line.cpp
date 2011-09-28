#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "scripts.h"

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
    //char* label_lists_file=NULL;
    while ( (c = getopt(argc, argv, "b:h")) != -1) {
        switch (c) {
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
        test(argv[optind+1],
             argv[optind+2],
             argv[optind+3],
             strtol(argv[optind+4],&strend,10),
             allowed_label_bigram_file);
        
    }
    
    return 0;

}
