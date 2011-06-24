#include<stdio.h>
#include<stdlib.h>

#include"scripts.h"

int main(int argc, char *argv[]){
    //only for development
    //train("training.bin","model.bin",5);
    //test("model.bin","test.bin","result.txt",1);
    //end of development code
    
    
    if(argc==1){
        //print some instructuion
        printf("per-- toolkit\n");
        printf("author: ZHANG Kaixu\n");
        printf("see http://code.google.com/p/perminusminus/ for more info.\n");
        return 0;
    }
    
    char* strend;
    if(argv[1][0]=='l'){
        train(argv[2],
                argv[3],
                strtol(argv[4],&strend,10));
    }else if(argv[1][0]=='p'){
        test(argv[2],
             argv[3],
             argv[4],
             strtol(argv[5],&strend,10));
        
    }
    
    return 0;

}
