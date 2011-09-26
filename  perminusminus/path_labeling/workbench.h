#ifndef __PATH_LABELING_H__
#define __PATH_LABELING_H__
#include<stdlib.h>
#include "model.h"
#include"decoder.h"
#include"graph.h"


struct PERMM{
    int length;
    int* values;
    Alpha_Beta* alphas;
    Alpha_Beta* betas;
    int n_best;
    int* result;
    Model* model;
    int steps;
    int eval_strandard_label;
    int eval_correct_label;
    int eval_result_label;
    PERMM(Model* model,int n_best=1){
        this->eval_reset();
        this->steps=0;
        this->length=10;
        this->n_best=n_best;
        this->model=model;
        this->values=(int*)calloc(4,length*this->model->l_size);
        this->result=(int*)calloc(4,n_best*length*this->model->l_size);
        this->alphas=(Alpha_Beta*)calloc(sizeof(Alpha_Beta),n_best*length*this->model->l_size);
        this->betas=(Alpha_Beta*)calloc(sizeof(Alpha_Beta),n_best*length*this->model->l_size);
        
    }
    void add_values(Graph* graph){
        int* p=graph->features;
        int fid=0;
        int l_size=this->model->l_size;
        int node_count=graph->node_count;
        while(node_count>this->length){//need to extends
            (this->length)*=2;
            this->values=(int*)realloc(this->values,4*this->length*this->model->l_size);
            this->result=(int*)realloc(this->result,n_best*4*this->length*this->model->l_size);
            this->alphas=(Alpha_Beta*)realloc(this->alphas,n_best*sizeof(Alpha_Beta)*length*this->model->l_size);
            this->betas=(Alpha_Beta*)realloc(this->betas,n_best*sizeof(Alpha_Beta)*length*this->model->l_size);
            
        }
        for(int i=0;i<node_count*this->model->l_size;i++){
            this->values[i]=0;
        }
        for(int i=0;i<n_best*node_count*this->model->l_size;i++){
            this->result[i]=-1;
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
        /**
         * 更新
         * 根据标签：
         * 非负数表示标准答案
         * -1表示无gold standard的标签，且需要更新
         * -2表示无gold standard的标签，但不更新
         * */
        int fid=0;
        int*p=graph->features;
        int l_size=this->model->l_size;
        for(int i=0;i<graph->node_count;i++){
            /* 如果正确就不动作
             * 当然如果标准答案标注为 -2, 也不动作
             * */
            if((graph->labels[i]==-2)||(graph->labels[i]==this->result[i])){
                while((*(p++))>=0);//但是要滑动特征指针
                continue;
            }
            while((fid=*(p++))>=0){//枚举对应位置的特征
                if(graph->labels[i]>=0){//如果是正确答案，特征权重提高，平均的也要加
                    this->model->fl_weights[fid*l_size+graph->labels[i]]++;
                    this->model->ave_fl_weights[fid*l_size+graph->labels[i]]+=
                        this->steps;
                }
                if(this->result[i]>=0){//如果是输出，特征权重降低，平均的也要减
                    this->model->fl_weights[fid*l_size+this->result[i]]--;
                    this->model->ave_fl_weights[fid*l_size+this->result[i]]-=
                        this->steps;
                }
            };
        }

        int*pip;
        int pi;
        int ll_ind;
        for(int i=0;i<graph->node_count;i++){//标准答案的bigram feature
            if(graph->labels[i]>=0){
                pip=graph->nodes[i].predecessors;
                pi=0;
                while((pi=*(pip++))>=0){
                    if(graph->labels[pi]>=0){
                        ll_ind=(graph->labels[pi])*l_size+graph->labels[i];
                        this->model->ll_weights[ll_ind]++;
                        this->model->ave_ll_weights[ll_ind]+=this->steps;
                    }
                }
            }
        }
        
        for(int i=0;i<graph->node_count;i++){//程序输出的bigram feature
            if(this->result[i]>=0){
                pip=graph->nodes[i].predecessors;
                pi=0;
                while((pi=*(pip++))>=0){
                    if(this->result[pi]>=0){
                        ll_ind=(this->result[pi])*l_size+this->result[i];
                        this->model->ll_weights[ll_ind]--;
                        this->model->ave_ll_weights[ll_ind]-=this->steps;
                    }
                }
            }
        }
        
        this->steps++;
        
    }
    void eval_reset(){
        this->eval_strandard_label=0;
        this->eval_correct_label=0;
        this->eval_result_label=0;
    }
    void eval_print(){
        printf("std: %d rst: %d cor:%d\n",this->eval_strandard_label,this->eval_result_label,
               this->eval_correct_label);
        double p=(double)(this->eval_correct_label)/(this->eval_result_label);
        double r=(double)(this->eval_correct_label)/(this->eval_strandard_label);
        double f=2*p*r/(p+r);
        printf("precesion: %f recall: %f f1: %f\n",p,r,f);
    }
    void eval(Graph*& graph){
        for(int i=0;i<graph->node_count;i++){
            if(graph->labels[i]>=0)
                this->eval_strandard_label++;
            if(this->result[i]>=0)
                this->eval_result_label++;
            if((graph->labels[i]>=0)&&(this->result[i]==graph->labels[i]))
                this->eval_correct_label++;
        }
        return;
    }
    void decode(Graph*& graph){
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

        this->eval(graph);
    }
    void nb_decode(Graph*& graph){
        this->add_values(graph);
        
        dp_nb_decode(
            this->model->l_size,
            this->model->ll_weights,
            graph->node_count,
            graph->nodes,
            this->values,
            this->n_best,
            this->alphas,
            this->result
        );
        this->eval(graph);
    }
    void cal_betas(Graph*& graph){
        int*buffer=dp_cal_successors(graph->node_count,graph->nodes);

        
        dp_cal_betas(
            this->model->l_size,
            this->model->ll_weights,
            graph->node_count,
            graph->nodes,
            this->values,
            this->betas
        );
        free(buffer);
    }
};




#endif
