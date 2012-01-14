#include<cstdlib>
#include<vector>
#include<list>
#include<cstdio>
#include<iostream>
#include<unistd.h>
#include"dat.h"

using namespace daidai;
bool compare_words (DATMaker::KeyValue& first, DATMaker::KeyValue& second)
{
    daidai::Word& first_word=first.key;
    daidai::Word& second_word=second.key;
    size_t min_size=(first_word.size()<second_word.size())?first_word.size():second_word.size();
    for(int i=0;i<min_size;i++){
        if(first_word[i]>second_word[i])return false;
        if(first_word[i]<second_word[i])return true;
    }
    
  return (first_word.size()<second_word.size());
}


void showhelp(){
    printf("双数组TRIE树构建器\n\t作者：张开旭\n");
    printf("    get words and make DAT\n");
    printf("-f filename\n    use filename instead of stdin\n");
    printf("-s\n    save base array and check array Seperately\n");
    printf("-P\n    申明没有一个词是另一个词的前缀，将编号存在base，而不是base指向的节点\n");
    
}
int main(int argc,char **argv){
    int c;
    int is_old_style=false;
    char* lexicon_filename=NULL;
    int no_prefix=0;
    while ( (c = getopt(argc, argv, "f:shP")) != -1) {
        switch (c) {
            case 's'://seperated two arrays
                is_old_style=true;
                break;
            case 'P'://prefix free
                no_prefix=true;
                break;
            case 'f' : //specify the file
                lexicon_filename = optarg;
                break;            
            case 'h' :
            case '?' : 
            default : 
                showhelp();
                return 1;
        }
    }
    char* dat_filename=argv[optind];
    
    FILE* inputFile=stdin;
    
    
    DATMaker dm;
    fprintf(stdout,"Double Array Trie Builder, author ZHANG, Kaixu\n");
    std::vector<DATMaker::KeyValue> lexicon;
    std::list<DATMaker::KeyValue> word_list;
    word_list.push_back(DATMaker::KeyValue());
    int end_character=0;
    
    //load wordlist
    int id=0;
    if(lexicon_filename){
        inputFile=fopen(lexicon_filename,"r");
    }
    do{
        end_character=daidai::get_raw(word_list.back().key,inputFile,32);//space is allowed
        if((int)word_list.back().key.size()>0){
            word_list.back().value=id;
            word_list.push_back(DATMaker::KeyValue());
            word_list.back().key.clear();
            id+=1;
        }
        if(end_character==-1)break;
    }while(1);
    if(lexicon_filename){
        fclose(inputFile);
    }
    word_list.pop_back();
    
    //sort
    word_list.sort(compare_words);
    //make it a vector
    for(std::list<DATMaker::KeyValue>::iterator it=word_list.begin();it!=word_list.end();++it){
        lexicon.push_back((*it));
    }
    

    
    
    fprintf(stdout,"%d words are loaded\n",(int)lexicon.size());
    dm.make_dat(lexicon,no_prefix);
    dm.shrink();
    fprintf(stdout,"size of DAT %d\n",(int)dm.dat_size);
    
    //save it
    if(is_old_style)
        dm.save_as_old_type(dat_filename);
    else
        dm.save_as(dat_filename);
    
};


