#include <list>
#include "tagging_decoder.h"

namespace daidai{

int TaggingDecoder::segment(int* input,int length,int* tags){
    if(not length)return 0;
    for(int i=0;i<length;i++){
        sequence[i]=input[i];
    }
    len=length;
    
    put_values();//检索出特征值并初始化放在values数组里
    dp();//动态规划搜索最优解放在result数组里
    
    for(int i=0;i<len;i++){
        if((label_info[result[i]][0]=='0')||(label_info[result[i]][0]=='3')){//分词位置
            tags[i]=1;
        }else{
            tags[i]=0;
        }
    }
}

TaggingDecoder::TaggingDecoder(){
    this->max_length=10000;
    this->len=0;
    this->sequence=new int[this->max_length];
    this->allowed_label_lists=new int*[this->max_length];
    
    this->uni_bases=new int[this->max_length+2];
    this->bi_bases=new int[this->max_length+4];
    
    bases=NULL;checks=NULL;
    
    nodes=new permm::Node[this->max_length];
    
    this->char_label_map=new int[65536];
    this->allowed_labels=NULL;
    this->label_trans=NULL;
    label_trans_pre=NULL;
    label_trans_post=NULL;
    this->threshold=10000;
    
    this->allow_sep=new int[this->max_length];
    this->allow_com=new int[this->max_length];
    
    this->tag_size=0;
    //this->is_good_choice=NULL;
    
    this->model=NULL;
    
    alphas=NULL;
    betas=NULL;
}
TaggingDecoder::~TaggingDecoder(){
    delete[]sequence;
    delete[]allowed_label_lists;
    
    delete[](uni_bases);
    delete[](bi_bases);
    

    free(bases);
    free(checks);
    
    for(int i=0;i<max_length;i++){
        delete[](nodes[i].predecessors);
        delete[](nodes[i].successors);
    }
    delete[](nodes);
    free(values);
    free(alphas);
    free(betas);
    free(result);
    if(model!=NULL)for(int i=0;i<model->l_size;i++){
        if(label_info)delete[](label_info[i]);
    };
    delete[](label_info);
    
        
    delete[](char_label_map);
    free(allowed_labels);
    
    free(label_trans);
    if(model!=NULL)for(int i=0;i<model->l_size;i++){
        if(label_trans_pre)free(label_trans_pre[i]);
        if(label_trans_post)free(label_trans_post[i]);
    }
    free(label_trans_pre);
    free(label_trans_post);
    
    delete[](allow_sep);
    delete[](allow_com);
    
    if(model!=NULL)for(int i=0;i<model->l_size;i++){
        if(label_looking_for)delete[](label_looking_for[i]);
    };
    delete[](label_looking_for);
    delete[](is_good_choice);
    
    if(model!=NULL)delete model;
}

/*初始化一个双数组trie树*/
void TaggingDecoder::load_da(char* filename,int* &base_array,int* &check_array,int &size){
    //打开文件
    FILE * pFile=fopen ( filename , "rb" );
    /*得到文件大小*/
    fseek (pFile , 0 , SEEK_END);
    size=ftell (pFile)/sizeof(int)/2;//双数组大小
    rewind (pFile);//重置文件指针到开头
    /*前一半是base数组，后一半是check数组*/
    base_array=(int*) malloc (sizeof(int)*size);
    check_array=(int*) malloc (sizeof(int)*size);
    int rtn=fread (base_array,sizeof(int),size,pFile);
    rtn=fread (check_array,sizeof(int),size,pFile);
    //关闭文件
    fclose (pFile);
};


void TaggingDecoder::init(
        char* model_file="model.bin",
        char* dat_file="dat.bin",
        char* label_file="label.txt",
        char* label_trans,
        char* label_lists_file){
    /**模型*/
    model=new permm::Model(model_file);
    
    /**解码用*/
    values=(int*)calloc(sizeof(int),max_length*model->l_size);
    alphas=(permm::Alpha_Beta*)calloc(sizeof(permm::Alpha_Beta),max_length*model->l_size);
    betas=(permm::Alpha_Beta*)calloc(sizeof(permm::Alpha_Beta),max_length*model->l_size);
    result=(int*)calloc(sizeof(int),max_length*model->l_size);
    label_info=new char*[model->l_size];
    
    for(int i=0;i<max_length;i++){
        int* pr=new int[2];
        pr[0]=i-1;
        pr[1]=-1;
        nodes[i].predecessors=pr;
        
        pr=new int[2];
        pr[0]=i+1;
        pr[1]=-1;
        nodes[i].successors=pr;
    };
    load_da(dat_file,bases,checks,dat_size);
    
    char* str=new char[16];
    FILE *fp;
    fp = fopen(label_file, "r");
    int ind=0;
    while( fscanf(fp, "%s", str)==1){
        label_info[ind]=str;
        str=new char[16];
        ind++;
    }
    delete[]str;
    fclose(fp);
    
    label_looking_for=new int*[model->l_size];
    for(int i=0;i<model->l_size;i++)
        label_looking_for[i]=NULL;
    for(int i=0;i<model->l_size;i++){
        if(label_info[i][0]=='0' || label_info[i][0]=='3')continue;
        
        for(int j=0;j<=i;j++){
            if((strcmp(label_info[i]+1,label_info[j]+1)==0)&&(label_info[j][0]=='0')){
                if(label_looking_for[j]==NULL){
                    label_looking_for[j]=new int[2];
                    label_looking_for[j][0]=-1;label_looking_for[j][1]=-1;
                    tag_size++;
                }
                label_looking_for[j][label_info[i][0]-'1']=i;
                break;
            }
        }
    }
    //printf("tagsize %d",tag_size);
    

    
    /**label_trans*/
    if(label_trans){
        load_label_trans(label_trans);
    }
    
    if(label_lists_file){
        load_label_lists(label_lists_file);
        
    }else{
        for(int i=0;i<max_length;i++)
            allowed_label_lists[i]=NULL;
    }
    
    is_good_choice=new int[max_length*model->l_size];
    
}

inline void TaggingDecoder::find_bases(int*bases,int*checks,int dat_size,int ch1,int ch2,int& uni_base,int&bi_base){
    if(checks[ch1]){
        uni_base=-1;bi_base=-1;return;
    }
    uni_base=bases[ch1]+32;
    int ind=bases[ch1]+ch2;
    if(ind>=dat_size||checks[ind]!=ch1){
        bi_base=-1;return;
    }
    bi_base=bases[ind]+32;
    
}
void TaggingDecoder::dp(){
    dp_decode(
            model->l_size,//check
            model->ll_weights,//check
            len,//check
            nodes,
            values,
            alphas,
            result,
            label_trans_pre,
            allowed_label_lists
        );
}
void TaggingDecoder::cal_betas(){
    int tmp=nodes[len-1].successors[0];
    nodes[len-1].successors[0]=-1;
    dp_cal_betas(
            model->l_size,
            model->ll_weights,
            len,
            nodes,
            values,
            betas
    );
    nodes[len-1].successors[0]=tmp;
}

void TaggingDecoder::load_label_lists(char*filename){
    //打开文件
    FILE * pFile=fopen ( filename , "rb" );
    /*得到文件大小*/
    int remain_size=0;
    int rtn=fread (&remain_size,sizeof(int),1,pFile);
    
    rtn=fread (char_label_map,sizeof(int),65536,pFile);
    allowed_labels=new int[remain_size-65536];
    rtn=fread (allowed_labels,sizeof(int),remain_size-65536,pFile);
    fclose (pFile);
    //printf("loaded");
    return;
}

void TaggingDecoder::set_label_trans(){
    int l_size=this->model->l_size;
    std::list<int> pre_labels[l_size];
    std::list<int> post_labels[l_size];
    for(int i=0;i<l_size;i++)
        for(int j=0;j<l_size;j++){
            // 0:B 1:M 2:E 3:S
            int ni=this->label_info[i][0]-'0';
            int nj=this->label_info[j][0]-'0';
            int i_is_end=((ni==2)//i is end of a word
                    ||(ni==3));
            int j_is_begin=((nj==0)//i is end of a word
                    ||(nj==3));
            int same_tag=strcmp(this->label_info[i]+1,this->label_info[j]+1);
            
            if(same_tag==0){
                if((ni==0&&nj==1)||
                        (ni==0&&nj==2)||
                        (ni==1&&nj==2)||
                        (ni==1&&nj==1)||
                        (ni==2&&nj==0)||
                        (ni==2&&nj==3)||
                        (ni==3&&nj==3)||
                        (ni==3&&nj==0)){
                    pre_labels[j].push_back(i);
                    post_labels[i].push_back(j);
                    //printf("ok\n");
                }
            }else{
                //printf("%s <> %s\n",this->label_info[i],this->label_info[j]);
                if(i_is_end&&j_is_begin){
                    pre_labels[j].push_back(i);
                    post_labels[i].push_back(j);
                    //printf("ok\n");
                }
            }
        }
    label_trans_pre=new int*[l_size];
    for(int i=0;i<l_size;i++){
        label_trans_pre[i]=new int[(int)pre_labels[i].size()+1];
        int k=0;
        //printf("i=%d>>\n",i);
        for(std::list<int>::iterator plist = pre_labels[i].begin();
                plist != pre_labels[i].end(); plist++){
            label_trans_pre[i][k]=*plist;
            //printf("%d ",*plist);
            k++;
        };
        //printf("\n");
        label_trans_pre[i][k]=-1;
    }
    
};

void TaggingDecoder::load_label_trans(char*filename){
    //打开文件
    FILE * pFile=fopen ( filename , "rb" );
    /*得到文件大小*/
    int remain_size=0;
    int rtn=fread (&remain_size,sizeof(int),1,pFile);
    /*得到矩阵数据*/
    label_trans=new int[remain_size];
    rtn=fread (label_trans,sizeof(int),remain_size,pFile);
    
    /*计算标签个数*/
    int label_size=0;
    for(int i=0;i<remain_size;i++){
        if(label_trans[i]==-1)label_size++;
    }
    label_size/=2;
    /*设定各个标签的指针*/
    label_trans_pre=new int*[label_size];
    label_trans_post=new int*[label_size];
    int ind=0;
    for(int i=0;i<label_size;i++){
        label_trans_pre[i]=label_trans+ind;
        while(label_trans[ind]!=-1)ind++;ind++;
        label_trans_post[i]=label_trans+ind;
        while(label_trans[ind]!=-1)ind++;ind++;
    }
    //printf("%d",remain_size);
    fclose (pFile);
    
    return;
}


inline void TaggingDecoder::add_values(int *value_offset,int base,int del,int* p_allowed_label=NULL){
    int ind=bases[base]+del;
    if(ind>=dat_size||checks[ind]!=base){
        return;
    }
    int offset=bases[bases[base]+del];
    int* weight_offset=model->fl_weights+offset*model->l_size;
    //p_allowed_label=NULL;
    int allowed_label;
    if(model->l_size==4){
        value_offset[0]+=weight_offset[0];
        value_offset[1]+=weight_offset[1];
        value_offset[2]+=weight_offset[2];
        value_offset[3]+=weight_offset[3];
    }else{
        if(p_allowed_label){
            while((allowed_label=(*(p_allowed_label++)))>=0){
                value_offset[allowed_label]+=weight_offset[allowed_label];
            }
        }else{
            for(int j=0;j<model->l_size;j++){
                value_offset[j]+=weight_offset[j];
            }
        }
    }
}
void TaggingDecoder::put_values(){
    if(!len)return;
    /*nodes*/
    for(int i=0;i<len;i++){
        nodes[i].type=0;
    }
    nodes[0].type+=1;
    nodes[len-1].type+=2;
    
    ///allowed_label_lists
    if(allowed_labels){
        for(int i=0;i<len;i++){
            if(char_label_map[sequence[i]]==-1){
                allowed_label_lists[i]=NULL;
            }else{
                allowed_label_lists[i]=allowed_labels+char_label_map[sequence[i]];
            }
        }
    }
    /*values*/
    for(int i=0;i<len*model->l_size;i++){
        values[i]=0;
    }
    find_bases(bases,checks,dat_size,35,35,uni_bases[0],bi_bases[0]);
    find_bases(bases,checks,dat_size,35,sequence[0],uni_bases[0],bi_bases[1]);
    for(int i=0;i+1<len;i++)
        find_bases(bases,checks,dat_size,sequence[i],sequence[i+1],uni_bases[i+1],bi_bases[i+2]);
    find_bases(bases,checks,dat_size,sequence[len-1],35,uni_bases[len],bi_bases[len+1]);
    find_bases(bases,checks,dat_size,35,35,uni_bases[len+1],bi_bases[len+2]);
    
    int base=0;
    for(int i=0;i<len;i++){
        int* value_offset=values+i*model->l_size;
        if((base=uni_bases[i+1])!=-1)
            add_values(value_offset,base,49,allowed_label_lists[i]);
        if((base=uni_bases[i])!=-1)
            add_values(value_offset,base,50,allowed_label_lists[i]);
        if((base=uni_bases[i+2])!=-1)
            add_values(value_offset,base,51,allowed_label_lists[i]);
        if((base=bi_bases[i+1])!=-1)
            add_values(value_offset,base,49,allowed_label_lists[i]);
        if((base=bi_bases[i+2])!=-1)
            add_values(value_offset,base,50,allowed_label_lists[i]);
        if((base=bi_bases[i])!=-1)
            add_values(value_offset,base,51,allowed_label_lists[i]);
        if((base=bi_bases[i+3])!=-1)
            add_values(value_offset,base,52,allowed_label_lists[i]);
    }
    
}


void TaggingDecoder::output_raw_sentence(){
    int c;
    for(int i=0;i<len;i++){
        c=sequence[i];
        if(c<128){//1个byte的utf-8
            putchar(c);
        }else if(c<0x800){//2个byte的utf-8
            putchar(0xc0|(c>>6));
            putchar(0x80|(c&0x3f));
        }else{//3个byte的utf-8
            putchar(0xe0|((c>>12)&0x0f));
            putchar(0x80|((c>>6)&0x3f));
            putchar(0x80|(c&0x3f));
        }
    }
}
void TaggingDecoder::output_sentence(){
    int c;
    for(int i=0;i<len;i++){
        c=sequence[i];
        if(c<128){//1个byte的utf-8
            putchar(c);
        }else if(c<0x800){//2个byte的utf-8
            putchar(0xc0|(c>>6));
            putchar(0x80|(c&0x3f));
        }else{//3个byte的utf-8
            putchar(0xe0|((c>>12)&0x0f));
            putchar(0x80|((c>>6)&0x3f));
            putchar(0x80|(c&0x3f));
        }
        //printf("(%d)",result[i]);

        if((label_info[result[i]][0]=='2')||(label_info[result[i]][0]=='3')){//分词位置
            if(*(label_info[result[i]]+1))//输出标签（如果有的话）
                printf("%s",label_info[result[i]]+1);
            if((i+1)<len)putchar(' ');//在分词位置输出空格
        }
    }
}

void TaggingDecoder::find_good_choice(){
    /*找到score最大值*/
    int max_score=alphas[0].value+betas[0].value-values[0];
    int max_ind=0;
    for(int i=1;i<model->l_size;i++){
        int score=alphas[i].value+betas[i].value-values[i];
        if(score>max_score){
            max_score=score;
            max_ind=i;
        }
    }
    
    /*找出可能的标注*/
    for(int i=0;i<len*model->l_size;i++)
        is_good_choice[i]=alphas[i].value+betas[i].value-values[i]+threshold>=max_score;
};

    
void TaggingDecoder::output_allow_tagging(){
    
    //threshold=15000;
    /*找到score最大值*/
    int max_score=alphas[0].value+betas[0].value-values[0];
    int max_ind=0;
    for(int i=1;i<model->l_size;i++){
        int score=alphas[i].value+betas[i].value-values[i];
        if(score>max_score){
            max_score=score;
            max_ind=i;
        }
    }
    //printf("max=%d\n",max_score);
    //printf("\n");
    /*找出可能的标注序*/
    for(int i=0;i<len*model->l_size;i++)
        is_good_choice[i]=alphas[i].value+betas[i].value-values[i]+threshold>=max_score;
    
    /*找出可能的词*/
    int this_score=0;
    int left_part=0;
    int last_id=0;
    for(int i=0;i<len;i++){
        //printf("i=%d\n",i);
        for(int b_label_i=0;b_label_i<model->l_size;b_label_i++){
            if(!is_good_choice[i*model->l_size+b_label_i]){
                continue;
            }
            if(label_info[b_label_i][0]=='3'){
                //输出单个字的词
                this_score=alphas[i*model->l_size+b_label_i].value
                        +betas[i*model->l_size+b_label_i].value
                        -values[i*model->l_size+b_label_i];
                printf("%d,%d,%s,%d ",i,i+1,label_info[b_label_i]+1,max_score-this_score);
            }else if(label_info[b_label_i][0]=='0'){
                int mid_ind=label_looking_for[b_label_i][0];
                int right_ind=label_looking_for[b_label_i][1];
                left_part=alphas[i*model->l_size+b_label_i].value;
                last_id=b_label_i;
                for(int j=i+1;j<len;j++){
                    if(j==len)break;
                    if(right_ind==-1)break;

                    if(is_good_choice[j*model->l_size+right_ind]){
                        //check，是不是合格的词
                        this_score=left_part
                                +model->ll_weights[last_id*model->l_size+right_ind]
                                +betas[j*model->l_size+right_ind].value;
                        if(max_score-this_score<=threshold)
                            printf("%d,%d,%s,%d ",i,j+1,label_info[b_label_i]+1,max_score-this_score);
                    }
                    if(mid_ind==-1)break;
                    if(!is_good_choice[(j*(model->l_size))+mid_ind])
                        break;
                    left_part+=values[j*model->l_size+mid_ind]
                            +model->ll_weights[last_id*model->l_size+mid_ind];
                    last_id=mid_ind;
                }
                
            }
        }
    }
}



/*对缓存里的串分词并编码成utf-8输出*/
void TaggingDecoder::output(int show_sentence){
    put_values();//检索出特征值并初始化放在values数组里
    dp();//动态规划搜索最优解放在result数组里
    
    if(threshold==0){
        output_sentence();
    }else{
        if(show_sentence){
            output_raw_sentence();
            printf(" ");
        }
        cal_betas();
        output_allow_tagging();
    }
}

int TaggingDecoder::get_input_from_stream(int*input,int max_length,int& length){
    int c;
    length=0;
    while(1){//反复读取输入流直到文件末尾
        //printf("nnn\n");
        c=getchar();
        if(c==EOF){
            if(len)return 0;//end of file
        }
        if(!(c&0x80)){//1个byte的utf-8编码
            if(c<=32){//非打印字符及空格
                if(length){
                    return 1;//对缓存中的串分词并输出
                }
                putchar(c);
            }else{//一般ascii字符
                input[length++]=c+65248;//半角转全角，放入缓存
            }
        }else if(!(c&0x20)){//2个byte的utf-8编码
            input[length++]=((c&0x1f)<<6)|
                (getchar()&0x3f);
        }else if(!(c&0x10)){//3个byte的utf-8编码
            input[length++]=((c&0x0f)<<12)|
                ((getchar()&0x3f)<<6)|
                (getchar()&0x3f);
        }else{//更大的unicode编码不能处理
            return -1;
        }
    }
};

}
