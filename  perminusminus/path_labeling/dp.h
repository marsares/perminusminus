#ifndef __DP_H__
#define __DP_H__
#include<stdlib.h>

//topological information about a node
struct Node{
    int type;//initial? terminal?
    int* predecessors;//ends with a -1
    int* successors;//ends with a -1
};


//a structure for alphas and betas
struct Alpha_Beta{
    int value;
    int label_id;
    int node_id;
};

/* The DP algorithm(s) for path labeling */

inline void dp_decode(
        //something about the model
        int l_size,
        int* ll_weights,
        //something about the graph
        int node_count,//numbers of nodes
        Node* nodes,
        //something about the scores
        int* values,//value for i-th node with j-th label
        Alpha_Beta* alphas,//alpha value (and the pointer) for i-th node with j-th label
        //something about the result
        int* result
        ){
    
    //calculate alphas
    int node_id;
    int* p_node_id;
    Alpha_Beta* tmp;
    Alpha_Beta best;best.node_id=-1;
    int score;
    
    for(int i=0;i<node_count;i++){//for each node
        for(int j=0;j<l_size;j++){//for each label
            tmp=&alphas[i*l_size+j];
            tmp->node_id=-2;//non-reach
            p_node_id=nodes[i].predecessors;
            while((node_id=*(p_node_id++))>=0){
                //printf("%d's predecessors %d\n",i,node_id);
                for(int k=0;k<l_size;k++){
                    if(alphas[node_id*l_size+k].node_id==-2)continue;
                    score=alphas[node_id*l_size+k].value+ll_weights[k*l_size+j];
                    if((tmp->node_id<0)||(score>tmp->value)){
                        tmp->value=score;
                        tmp->node_id=node_id;
                        tmp->label_id=k;
                    }
                }
            }
            tmp->value+=values[i*l_size+j];
            
            if((nodes[i].type==1)||(nodes[i].type==3))
                tmp->node_id=-1;
            //printf("%d tmp %d %d ::: %d  %d id %d\n",nodes[i].type,i,j,tmp->value,tmp->node_id,tmp->label_id);
            //printf("best %d %d %d\n",best.value,best.node_id,best.label_id);
            if(nodes[i].type>=2){
                if((best.node_id==-1)||(best.value<tmp->value)){
                    best.value=tmp->value;
                    best.node_id=i;
                    best.label_id=j;
                }
            }
            //printf("best %d %d %d\n",best.value,best.node_id,best.label_id);
        }
    }
    //printf("best %d %d %d\n",best.value,best.node_id,best.label_id);
    //getchar();
    //find the path and label the nodes of it.
    for(node_id=0;node_id<node_count;node_id++)
        result[node_id]=-1;
    tmp=&best;
    //printf("node id %d label id %d\n",tmp->node_id,tmp->label_id);
    while(tmp->node_id>=0){
        
        result[tmp->node_id]=tmp->label_id;
        tmp=&(alphas[(tmp->node_id)*l_size+(tmp->label_id)]);
        //printf("%d %d\n",tmp->node_id,tmp->label_id);
    }
    return;
};
#endif
