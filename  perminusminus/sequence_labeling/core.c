#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dp.h"

const int MAX_LINE_LENGTH=1024;
const int MAX_SEQUENCE_LENGTH=10000;
const int MAX_VECTOR_LENGTH=1024;
const int DECIMAL_PLACES=1000;//decimal places 

int sequence[MAX_SEQUENCE_LENGTH*MAX_VECTOR_LENGTH];
int tags[MAX_SEQUENCE_LENGTH];

int t_size=0;
int f_size=0;
int s_size=0;

int* features;
double* ave_features;
int* trans_features;
double* ave_trans_features;
int ave_weight=0;

/*for decoder*/
int output[MAX_SEQUENCE_LENGTH];
int* values[MAX_SEQUENCE_LENGTH];
int* pointers[MAX_SEQUENCE_LENGTH];
int* alphas[MAX_SEQUENCE_LENGTH];
int* betas[MAX_SEQUENCE_LENGTH];
int n_best;
int** n_best_outputs;

candidate* n_best_candidates;


int token_count;
int correct_token_count;

FILE* output_file;

struct{
    int output_type;
} config={0};



void use_ave(int s){
    for(int i=0;i<t_size;i++){
        for(int j=0;j<t_size;j++){
            trans_features[i*t_size+j]=
                (int)((double)ave_trans_features[i*t_size+j]/s*DECIMAL_PLACES);
        }
    }
    for(int i=0;i<f_size;i++){
        for(int j=0;j<t_size;j++){
            features[i*t_size+j]=(int)((double)ave_features[i*t_size+j]/s*DECIMAL_PLACES);
        }
    }
}

void save_model(char* filename){
    //1+1+t*t+f*t
    FILE * pFile;
    pFile = fopen ( filename , "wb" );
    fwrite(&t_size,1,sizeof(int),pFile);
    fwrite(&f_size,1,sizeof(int),pFile);
    fwrite (trans_features,t_size*t_size,sizeof(int),pFile);
    fwrite (features,f_size*t_size,sizeof(int),pFile);
    fclose (pFile);
}
void load_model(char* filename){
    FILE * pFile;
    pFile = fopen ( filename , "rb" );
    fread(&t_size,1,sizeof(int),pFile);
    fread(&f_size,1,sizeof(int),pFile);
    features=new int[f_size*t_size];
    trans_features=new int[t_size*t_size];
    printf("%d %d\r\n",t_size,f_size);
    fread(trans_features,t_size*t_size,sizeof(int),pFile);
    fread(features,f_size*t_size,sizeof(int),pFile);
    fclose (pFile);
    if(config.output_type<=1){
        for(int i=0;i<MAX_SEQUENCE_LENGTH;i++){
            values[i]=new int[t_size];
            pointers[i]=new int[t_size];
            alphas[i]=new int[t_size];
            betas[i]=new int[t_size];
        }
    }else{
        n_best=config.output_type;
        n_best_candidates=new candidate[t_size*n_best];
        for(int i=0;i<MAX_SEQUENCE_LENGTH;i++){
            values[i]=new int[t_size*n_best];
            pointers[i]=new int[t_size*n_best];
        }
        n_best_outputs=new int*[n_best];
        for(int i=0;i<n_best;i++)
            n_best_outputs[i]=new int[MAX_SEQUENCE_LENGTH];
    }
}


struct Sequence_Generator{
    FILE *input_file;
    char mystring[MAX_LINE_LENGTH];
    int* sequence;
    Sequence_Generator(char* filename,int*sequence){
        this->input_file=fopen(filename,"r");
        this->sequence=sequence;
    };
    int gen_sequence(){
        int seq_len=0;
        while(fgets (this->mystring ,MAX_LINE_LENGTH,this->input_file)){
            char* pEnd=mystring;
            if(((*pEnd)=='\n')||((*pEnd)=='\r')){//end of a sequence
                if(seq_len)
                    return seq_len;
                continue;
            }
            int vid=0;
            tags[seq_len]=strtol(pEnd,&pEnd,10);
            while(((*pEnd)!='\n')&&((*pEnd)!='\r')){
                this->sequence[seq_len*MAX_VECTOR_LENGTH+vid]=strtol(pEnd,&pEnd,10);
                vid++;
            }
            this->sequence[seq_len*MAX_VECTOR_LENGTH+vid]=-1;
            seq_len++;
        }
        if(seq_len){
            return seq_len;
        }
        fclose (this->input_file);
        return 0;
    };
};

void load_sequence(char* filename,void(*callback)(int)){
    Sequence_Generator sg=Sequence_Generator(filename,sequence);
    int seq_len=0;
    while(seq_len=sg.gen_sequence()){
        callback(seq_len);
    }
}

 
/*count t_size, f_size*/
void count_size(int seq_len){
    s_size++;
    for(int i=0;i<seq_len;i++){
        if(t_size<tags[i])
            t_size=tags[i];
        int vid=0;
        while(sequence[i*MAX_VECTOR_LENGTH+vid]>=0){
            if(f_size<sequence[i*MAX_VECTOR_LENGTH+vid])
                f_size=sequence[i*MAX_VECTOR_LENGTH+vid];
            vid++;
        }
        //printf("%d\n",sequence[i*MAX_VECTOR_LENGTH+vid]);
    }
}


inline void refresh_counts(int seq_len){
    token_count+=seq_len;
    for(int i=0;i<seq_len;i++)
        if(tags[i]==output[i])
            correct_token_count++;
}

void decode(int seq_len){
    //add values
    int fid=0;
    for(int i=0;i<seq_len;i++){
        int vid=0;
        memset(values[i],0,t_size*sizeof(int));
        memset(pointers[i],0,t_size*sizeof(int));
        while((fid=sequence[i*MAX_VECTOR_LENGTH+vid])>=0){
            for(int j=0;j<t_size;j++)
                values[i][j]+=features[fid*t_size+j];
            vid++;
        }
    }
    dp_decode(seq_len,t_size,trans_features,output,values,pointers);
    refresh_counts(seq_len);
}

/*this is forwards and backwords*/
void decode2(int seq_len){
    //add values
    int fid=0;
    for(int i=0;i<seq_len;i++){
        int vid=0;
        memset(values[i],0,t_size*sizeof(int));
        memset(alphas[i],0,t_size*sizeof(int));
        memset(betas[i],0,t_size*sizeof(int));
        while((fid=sequence[i*MAX_VECTOR_LENGTH+vid])>=0){
            for(int j=0;j<t_size;j++)
                values[i][j]+=features[fid*t_size+j];
            vid++;
        }
    }
    dp_decode2(seq_len,t_size,trans_features,output,values,alphas,betas);
    refresh_counts(seq_len);
}


void n_best_decode(int seq_len){
    
    //add values
    int fid=0;
    for(int i=0;i<seq_len;i++){
        int vid=0;
        memset(values[i],0,t_size*n_best*sizeof(int));
        for(int j=0;j<t_size*n_best;j++)
            pointers[i][j]=-1;
        for(int j=0;j<t_size;j++)
            pointers[i][j*n_best]=0;
        while((fid=sequence[i*MAX_VECTOR_LENGTH+vid])>=0){
            for(int j=0;j<t_size;j++)
                values[i][j*n_best]+=features[fid*t_size+j];
            vid++;
        }
    }
    dp_n_best_decode(seq_len,t_size,trans_features,output,values,pointers
              ,n_best,n_best_candidates,n_best_outputs);
    refresh_counts(seq_len);
}


void update(int seq_len){
    int vid=0;
    int fid=0;
    for(int i=0;i<seq_len;i++){
        if(tags[i]==output[i]){
            continue;
        }
        vid=0;
        while((fid=sequence[i*MAX_VECTOR_LENGTH+vid])>=0){
            features[fid*t_size+output[i]]--;
            features[fid*t_size+tags[i]]++;
            ave_features[fid*t_size+output[i]]-=ave_weight;
            ave_features[fid*t_size+tags[i]]+=ave_weight;
            vid++;
        }    
    }
    for(int i=0;i<seq_len-1;i++){
        if((tags[i]==output[i])&&(tags[i+1]==output[i+1]))continue;
        trans_features[output[i]*t_size+output[i+1]]--;
        trans_features[tags[i]*t_size+tags[i+1]]++;
        ave_trans_features[output[i]*t_size+output[i+1]]-=ave_weight;
        ave_trans_features[tags[i]*t_size+tags[i+1]]+=ave_weight;
    }
    ave_weight--;
}


void learn_seq(int seq_len){
    decode(seq_len);
    
    update(seq_len);
}

void train(char* trainingfilename,char*modelfilename,int N=10){
    //initilize
    t_size=0;f_size=0;s_size=0;
    load_sequence(trainingfilename,&count_size);
    t_size++;f_size++;ave_weight=N*s_size;
    printf("labels: %d\r\n",t_size);
    printf("features: %d\r\n",f_size);
    printf("training set size: %d\r\n",s_size);
    features=new int[f_size*t_size];
    memset(features,0,f_size*t_size*sizeof(int));
    ave_features=new double[f_size*t_size];
    memset(ave_features,0,f_size*t_size*sizeof(double));
    trans_features=new int[t_size*t_size];
    memset(trans_features,0,t_size*t_size*sizeof(int));
    ave_trans_features=new double[t_size*t_size];
    memset(ave_trans_features,0,t_size*t_size*sizeof(double));
    for(int i=0;i<MAX_SEQUENCE_LENGTH;i++){
        values[i]=new int[t_size];
        pointers[i]=new int[t_size];
    }
    
    for(int i=0;i<N;i++){
        token_count=0;
        correct_token_count=0;
        load_sequence(trainingfilename,&learn_seq);
        printf("%d %d %d %f\r\n",i+1,token_count,correct_token_count,(double)correct_token_count/token_count);
    }
    use_ave(N*s_size);
    save_model(modelfilename);
}
void test_seq(int seq_len){
    //decode(seq_len);
    //for(int i=0;i<seq_len;i++){
    //    fprintf(output_file,"%d %d\n",tags[i],output[i]);
    //}
    if(config.output_type>0){
        int ind=0;
        n_best_decode(seq_len);
        for(int i=0;i<seq_len;i++){
            fprintf(output_file,"%d",tags[i]);
            for(ind=0;ind<n_best;ind++){
                if(n_best_outputs[ind][0]<0)break;
                fprintf(output_file," %d",n_best_outputs[ind][i]);
            }
            fprintf(output_file,"\n");
        }
        fprintf(output_file,"\n");
        return;
    
    }
    if(config.output_type==0){
        decode(seq_len);
    }else{
        decode2(seq_len);
    }
    for(int i=0;i<seq_len;i++){
        if(config.output_type==0){
            fprintf(output_file,"%d",output[i]);
        }else if(config.output_type==-1){
            fprintf(output_file,"%d %d",tags[i],output[i]);
        }else if(config.output_type==-2){
            fprintf(output_file,"%d %d",tags[i],output[i]);
            for(int j=0;j<t_size;j++){
                fprintf(output_file," %g",
                    (double)(alphas[i][j]+betas[i][j]-values[i][j])/DECIMAL_PLACES
                    );
            }
        }else if(config.output_type==-3){
            fprintf(output_file,"%d %d",tags[i],output[i]);
            for(int j=0;j<t_size;j++){
                fprintf(output_file," %g/%g/%g",
                    (double)alphas[i][j]/DECIMAL_PLACES,
                    (double)values[i][j]/DECIMAL_PLACES,
                    (double)betas[i][j]/DECIMAL_PLACES);
            }
        }
        fprintf(output_file,"\n");
    }
    fprintf(output_file,"\n");
    
}
void test(char*modelfilename,char*src,char*dst){
    load_model(modelfilename);
    token_count=0;
    correct_token_count=0;
    output_file = fopen ( dst , "w" );
    load_sequence(src,&test_seq);
    
    printf("%d %d %f\r\n",token_count,correct_token_count,(double)correct_token_count/token_count);
}

int main(int argc,char* args[]){
    //for test of the code
    //train("test.txt","model.bin",5);
    //train("train.txt","model.bin",20);
    //test("model.bin","test.txt","result.txt");
    
    
    if(argc==1){
        printf("usage\n");
        printf("learn:\n");
        printf("  per--.exe l <training file> <iterations> <model file>\n");
        printf("predict:\n");
        printf("  per--.exe p <model file> <test file> <result file> <output type>\n");
        return 0;
    }
    char *strend;
    if(argc>=5){
        if(args[1][0]=='l'){
            train(args[2],
                     args[4],
                     strtol(args[3],&strend,10));
        }
        if(args[1][0]=='p'){
            if(argc>=6){
                config.output_type=strtol(args[5],&strend,10);
            }
            test(args[2],args[3],args[4]);
        }
        
    }
    return 0;
}
