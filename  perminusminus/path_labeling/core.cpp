#include<stdio.h>
#include<stdlib.h>
#include "graph_file.h"
#include "core.h"
#include"path_labeling.h"


void count_size(Graph_Loader& gl,int&l_size,int&f_size,int&g_size){
    Graph* graph;
    l_size=0;
    f_size=0;
    g_size=0;
    int nc=0;
    int* pfid;
    int fid;
    while(gl.load(graph)){
        g_size++;
        nc=graph->node_count;
        pfid=graph->features;
        for(int i=0;i<nc;i++){
            if(l_size<graph->labels[i])
                l_size=graph->labels[i];
            while((fid=*(pfid++))>=0){
                if(f_size<fid)f_size=fid;
            }
        }
        //delete graph;
    }
    l_size++;f_size++;
};



void test(char* modelfile,char*testfile,char*resultfile,int decode_type){
    Model* model=new Model(modelfile);
    printf("model loaded\n");
    PERMM permm(model,(decode_type>0?decode_type:1));
    Graph_Loader* gl;
    FILE* pFile=fopen(resultfile,"w");
    Graph* graph=NULL;
    
    permm.eval_reset();
    gl=new Graph_Loader(testfile);
    while(gl->load(graph)){
        //decode
        if(decode_type>0){
            permm.nb_decode(graph);
        }else{
            permm.decode(graph);
            if(decode_type<0)permm.cal_betas(graph);
        }
        //output
        if(decode_type>0){
            for(int i=0;i<graph->node_count;i++){
                fprintf(pFile,"%d",graph->labels[i]);
                for(int n=0;n<decode_type;n++){
                    fprintf(pFile," %d",permm.result[n*graph->node_count+i]);
                }
                fprintf(pFile,"\n");
            };
            
        }
        if(decode_type==0){//basic output
            for(int i=0;i<graph->node_count;i++){
                fprintf(pFile,"%d %d\n",graph->labels[i],permm.result[i]);
            };
        }
        if(decode_type==-1){//marginal score
            for(int i=0;i<graph->node_count;i++){
                fprintf(pFile,"%d %d",graph->labels[i],permm.result[i]);
                for(int j=0;j<model->l_size;j++){
                    fprintf(pFile," %g",
                            (double)(permm.alphas[i*model->l_size+j].value
                            -permm.values[i*model->l_size+j]
                            +permm.betas[i*model->l_size+j].value)/DEC
                            );
                }
                fprintf(pFile,"\n");
            };
        }
        if(decode_type==-2){//alpha value beta
            for(int i=0;i<graph->node_count;i++){
                fprintf(pFile,"%d %d",graph->labels[i],permm.result[i]);
                for(int j=0;j<model->l_size;j++){
                    fprintf(pFile," %g/%g/%g",
                            (double)(permm.alphas[i*model->l_size+j].value)/DEC,
                            (double)(permm.values[i*model->l_size+j])/DEC,
                            (double)(permm.betas[i*model->l_size+j].value)/DEC
                            );
                }
                fprintf(pFile,"\n");
            };
        }
        
        fprintf(pFile,"\n");//don't forget the newline for each graph
        
    }
    
    permm.eval_print();
    //fclose(pFile);
    
}


struct Training_Config{
    
};

void train(char* trainingfile,char* modelfile,int iterations){
    Graph_Loader* gl=new Graph_Loader(trainingfile);
    int l_size;int f_size;int g_size;
    count_size(*gl,l_size,f_size,g_size);
    printf("model initialized\n");
    printf("number of labels: %d\n",l_size);
    printf("number of features: %d\n",f_size);
    Model* model=new Model(l_size,f_size);
    
    PERMM permm(model);
    
    Graph* graph=NULL;
    for(int t=0;t<iterations;t++){
        printf("iteration %d\n",(t+1));
        permm.eval_reset();
        while(gl->load(graph)){
            permm.decode(graph);
            permm.update(graph);
        }
        permm.eval_print();


    }
    model->average(permm.steps);
    model->save(modelfile);
}
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

