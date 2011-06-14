#ifndef __DP_H__
#define __DP_H__
#include<stdlib.h>

//topological information about a node
struct Node{
    int type;//initial? terminal?
    int* predecessors;//ends with a -1
    int* successors;//ends with a -1
};

/**given prececessors, calculate successors*/
int* dp_cal_successors(int node_count,Node* nodes){
    int size=0;
    int* out_degrees=(int*)calloc(sizeof(int),node_count);
    int* p_node_id;
    int node_id;
    for(int i=0;i<node_count;i++){
        p_node_id=nodes[i].predecessors;
        while((node_id=*(p_node_id++))>=0){
            out_degrees[node_id]++;
        }
    }
    for(int i=0;i<node_count;i++){
        node_id=out_degrees[i]+1;
        out_degrees[i]=size;
        size+=node_id;
    }
    int*buffer=(int*)malloc(sizeof(int)*size);
    for(int i=0;i<size;i++)buffer[i]=-1;
    
    //for(int i=0;i<size;i++)printf("%d ",buffer[i]);
    //printf("\n");
    
    for(int i=0;i<node_count;i++){
        nodes[i].successors=buffer+out_degrees[i];
        out_degrees[i]=0;
    }
    for(int i=0;i<node_count;i++){
        p_node_id=nodes[i].predecessors;
        while((node_id=*(p_node_id++))>=0){
            nodes[node_id].successors[out_degrees[node_id]++]=i;
        }
    }
    //for(int i=0;i<size;i++)printf("%d ",buffer[i]);
    //printf("\n");
    //getchar();
    free(out_degrees);
    return buffer;
};

//a structure for alphas and betas
struct Alpha_Beta{
    int value;
    int label_id;
    int node_id;
};

/*the n-best heap for the n-best searching
 * */
inline void nb_heap_insert(Alpha_Beta* heap,int size,Alpha_Beta* element);

/**remove*/
inline Alpha_Beta* nb_heap_remove(Alpha_Beta* heap,int size);

/** The DP algorithm(s) for path labeling */
inline void dp_decode(
        /**something about the model*/
        int l_size,
        int* ll_weights,
        /**something about the graph*/
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
            if(nodes[i].type>=2){
                if((best.node_id==-1)||(best.value<tmp->value)){
                    best.value=tmp->value;
                    best.node_id=i;
                    best.label_id=j;
                }
            }
        }
    }
    //find the path and label the nodes of it.
    for(node_id=0;node_id<node_count;node_id++)
        result[node_id]=-1;
    tmp=&best;
    while(tmp->node_id>=0){
        result[tmp->node_id]=tmp->label_id;
        tmp=&(alphas[(tmp->node_id)*l_size+(tmp->label_id)]);
    }
    return;
};

inline void dp_cal_betas(
        /**something about the model*/
        int l_size,
        int* ll_weights,
        /**something about the graph*/
        int node_count,//numbers of nodes
        Node* nodes,
        //something about the scores
        int* values,//value for i-th node with j-th label
        Alpha_Beta* betas//alpha value (and the pointer) for i-th node with j-th label
        ){
    
    int node_id;
    int* p_node_id;
    Alpha_Beta* tmp;
    int score=0;
    
    for(int i=node_count-1;i>=0;i--){//for each node
        for(int j=0;j<l_size;j++){//for each label
            
            tmp=&betas[i*l_size+j];
            tmp->node_id=-2;//non-reach
            
            p_node_id=nodes[i].successors;
            while((node_id=*(p_node_id++))>=0){
                //printf("%d %d\n",i,node_id);
                for(int k=0;k<l_size;k++){
                    //printf("%d\n",betas[node_id*l_size+k].node_id);
                    if(betas[node_id*l_size+k].node_id==-2)continue;
                    
                    
                    score=betas[node_id*l_size+k].value+ll_weights[j*l_size+k];
                    
                    if((tmp->node_id<0)||(score>tmp->value)){
                        tmp->value=score;
                        tmp->node_id=node_id;
                        tmp->label_id=k;
                    }
                    
                }
            }
            
            tmp->value+=values[i*l_size+j];
            
            if((nodes[i].type==2)||(nodes[i].type==3))
                tmp->node_id=-1;
        }
    }
    return;
};

#endif
