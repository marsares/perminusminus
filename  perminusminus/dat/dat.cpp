#include"dat.h"
#include<cstdlib>
#include<vector>
#include<cstdio>
#include<iostream>
#include<unistd.h>


daidai::Word word;

void call_back(int b,int e){
    putchar(' ');
    for(int i=b;i<e;i++){
        daidai::put_character(word[i]);
    }
}

int main(int argc,char **argv){
    int c;
    int is_matching=true;
    while ( (c = getopt(argc, argv, "s")) != -1) {
        switch (c) {
            case 's':
                is_matching=false;
                break;
            //case 'b' : ///label binary的约束
                //label_trans = optarg;
                //break;
            //case 'u' : ///label unary的约束
                //label_lists_file = optarg;
                //break;
            //case 't' : ///output word lattice
                //tagging_decoder->threshold = atoi(optarg)*1000;
                //break;
            //case 's':///输出lattice之前输出原句
                //sl_decoder_show_sentence=1;
                //break;
            //case 'h' :
            //case '?' : 
            //default : 
                //showhelp();
                //return 1;
        }
    }
    char* dat_filename=argv[optind];
    
    
    
    DAT dat(dat_filename);
    fprintf(stdout,"Double Array Trie Tester, author ZHANG, Kaixu\n");
    
    
    int end_character=0;
    do{
        end_character=daidai::get_raw(word);
        if(word.size()){
            if(is_matching){
                int rtn=dat.match(word);
                if(rtn==-1){
                    std::cout<<"not matched";
                }else{
                    std::cout<<rtn<<" "<<dat.dat[rtn].base;
                }
                
            }else{
                dat.search(word,&call_back);
            }
        }
        if(end_character==-1)break;
        putchar(end_character);
    }while(1);
    
};

