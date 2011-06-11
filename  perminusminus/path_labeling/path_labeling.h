#ifndef __PATH_LABELING_H__
#define __PATH_LABELING_H__
#include<stdlib.h>
#include "core.h"
#include"dp.h"


struct PERMM{
    int length;
    int* values;
    Alpha_Beta* alphas;
    int* result;
    Model* model;
    int node_count;
    int correct_node;
    PERMM(Model* model,int length=10){
        this->node_count=0;
        this->correct_node=0;
        this->length=length;
        this->model=model;
        this->values=(int*)calloc(4,length*this->model->l_size);
        this->result=(int*)calloc(4,length*this->model->l_size);
        this->alphas=(Alpha_Beta*)calloc(sizeof(Alpha_Beta),length*this->model->l_size);
        
    }
    void add_values(Graph* graph){
        int* p=graph->features;
        int fid=0;
        int l_size=this->model->l_size;
        int node_count=graph->node_count;
        while(node_count>this->length){//need to extends
            (this->length)*=2;
            //printf("%d\n",node_count);
            this->values=(int*)realloc(this->values,4*this->length*this->model->l_size);
            this->result=(int*)realloc(this->result,4*this->length*this->model->l_size);
            this->alphas=(Alpha_Beta*)realloc(this->alphas,sizeof(Alpha_Beta)*length*this->model->l_size);
            
        }
        for(int i=0;i<this->length*this->model->l_size;i++){
            this->values[i]=0;
            this->result[i]=-1;
            this->alphas[i].value=0;
            this->alphas[i].node_id=-2;
            this->alphas[i].label_id=0;
        }
        for(int i=0;i<graph->node_count;i++){
            while((fid=*(p++))>=0){
                for(int j=0;j<this->model->l_size;j++){
                    this->values[i*l_size+j]+=
                        this->model->fl_weights[fid*l_size+j];
                }
            }
        }
        //for(int i=0;i<graph->node_count;i++){
        //    printf("%d %d %d\n",graph->labels[i],this->values[i*l_size],this->values[i*l_size+1]);
        //}
    };
    void update(Graph* graph){
        int fid=0;
        int*p=graph->features;
        int l_size=this->model->l_size;
        this->node_count+=graph->node_count;
        //this->correct_node+=graph->node_count;
        for(int i=0;i<graph->node_count;i++){
            if(graph->labels[i]==this->result[i]){
                this->correct_node++;
                //while((*(p++))>=0);
                //continue;
            }
            while((fid=*(p++))>=0){
                if(graph->labels[i]>=0)
                    this->model->fl_weights[fid*l_size+graph->labels[i]]++;
                if(this->result[i]>=0)
                    this->model->fl_weights[fid*l_size+this->result[i]]--;
            };
        }
        
        int pre_label=-1;
        int pre_result=-1;
        for(int i=0;i<graph->node_count;i++){
            if(graph->labels[i]>=0){
                if(pre_label>=0)
                    this->model->ll_weights[pre_label*l_size+graph->labels[i]]++;
                pre_label=graph->labels[i];
            }
            if(this->result[i]>=0){
                if(pre_result>=0)
                    this->model->ll_weights[pre_result*l_size+this->result[i]]--;
                pre_result=this->result[i];
            }
        }
    }
    void decode(Graph* graph){
        this->add_values(graph);
        
        dp_decode(
            this->model->l_size,
            this->model->ll_weights,
            graph->node_count,
            graph->nodes,
            this->values,
            this->alphas,
            this->result
        );
        //update(graph);
        /*
        for(int i=0;i<graph->node_count;i++){
            printf("%d ",graph->nodes[i].type);
        }printf("\n");
        for(int i=0;i<graph->node_count;i++){
            printf("%d ",graph->labels[i]);
        }printf("\n");
        for(int i=0;i<graph->node_count;i++){
            printf("%d ",this->result[i]);
        }printf("\n");*/
        
        
    }
};




#endif
