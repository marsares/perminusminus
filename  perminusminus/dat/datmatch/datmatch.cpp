#include"datmatch.h"
#include"UTF8.h"
#include <unistd.h>

int print_sentence=0;


void do_match(char* dat_file){
    int* bases;
    int* checks;
    int dat_size;
    dat_load(dat_file,bases,checks,dat_size);
    
    int mlength=1000;
    int* cache=new int(mlength);
    int length=0;
    
    int rtn=0;
    int match=0;
    while(1){
        rtn=utf8_stdin_readline(cache,mlength,length);
        if(length){
            match=dat_print_match(bases,checks,dat_size,cache,length);
            if(match){
                if(print_sentence){
                    putchar(' ');
                    utf8_stdout_write(cache,length);
                }
                if(rtn>0||rtn==-2){
                    putchar(rtn);
                }
            }
        }
        if(rtn==EOF){
            break;
        }
        
        
    }
}

void showhelp(){
    char *help = "DAT match by zkx@thunlp\n"
        "\n"
        "datmatch dat_bin_file\n"
        "-s     output original sentence after indexes\n"
        "-h     print this help and exit\n"
        "\n";
    printf("%s", help);
}


int main(int argc, char *argv[]){
    if(argc==1){
        showhelp();
        return 0;
    }
    int c;
    while ( (c = getopt(argc, argv, "sh")) != -1) {
        switch (c) {
            case 's' : 
                print_sentence = 1;
                break;
            
            case 'h' :
            case '?' : 
            default : 
                showhelp();
                return 1;
        }
    }
    
    do_match(argv[argc-1]);
    
    
    return 0;
}
