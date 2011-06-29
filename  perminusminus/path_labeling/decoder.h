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
    int node_id;
    int label_id;
    
};
int alpha_beta_comp(const void* a,const void* b){
    return ((Alpha_Beta*)b)->value-((Alpha_Beta*)a)->value;
}
/*
the n-best heap for the n-best searching
This is a min-heap
*/
inline void nb_heap_insert(Alpha_Beta* heap,int max_size,int& count,Alpha_Beta& element){
    if(count==max_size){//heap is full
        if(element.value<heap[0].value)return;//do not insert
        //remove the top and insert it
        //top_down;
        int ind=0;
        while(1){
            if((ind*2+1)<count){//has children
                if(heap[ind*2+1].value<element.value){//left is smaller
                    if(((ind*2+2)<count)&&(heap[ind*2+2].value<heap[ind*2+1].value)){
                        heap[ind]=heap[ind*2+2];
                        ind=ind*2+2;
                    }else{
                        heap[ind]=heap[ind*2+1];
                        ind=ind*2+1;
                    }
                }else{
                    if(((ind*2+2)<count)&&(heap[ind*2+2].value<element.value)){
                        heap[ind]=heap[ind*2+2];
                        ind=ind*2+2;
                    }else{
                        break;
                    }
                }
            }else{
                break;
            }
        }
        heap[ind]=element;
    }else{
        int ind=count++;
        //buttom up
        while(1){
            if(ind==0)break;
            if(heap[(ind-1)/2].value<=element.value)break;
            heap[ind]=heap[(ind-1)/2];
            ind=(ind-1)/2;
        }
        heap[ind]=element;
    }
};

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
            tmp->node_id=-2;//non reachable
            tmp->value=0;
            p_node_id=nodes[i].predecessors;
            while((node_id=*(p_node_id++))>=0){
                for(int k=0;k<l_size;k++){
                    if(alphas[node_id*l_size+k].node_id==-2)continue;//not reachable
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
    //for(node_id=0;node_id<node_count;node_id++)
    //    result[node_id]=-1;
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
            tmp->value=0;
            p_node_id=nodes[i].successors;
            while((node_id=*(p_node_id++))>=0){
                for(int k=0;k<l_size;k++){
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


inline void dp_nb_decode(
        /**something about the model*/
        int l_size,
        int* ll_weights,
        /**something about the graph*/
        int node_count,//numbers of nodes
        Node* nodes,
        //something about the scores
        int* values,//value for i-th node with j-th label
        int nb,//n-best
        Alpha_Beta* alphas,//alpha value (and the pointer) for i-th node with j-th label
        //something about the result
        int* result
        ){
    
    //calculate alphas
    int node_id;
    int* p_node_id;
    Alpha_Beta best[nb];
    int best_count=0;
    int score;
    int count;
    for(int i=0;i<node_count;i++){//for each node
        for(int j=0;j<l_size;j++){//for each label
            p_node_id=nodes[i].predecessors;
            count=0;
            for(int n=0;n<nb;n++){
                alphas[(i*l_size+j)*nb+n].node_id=-2;
                alphas[(i*l_size+j)*nb+n].value=0;
            }
            while((node_id=*(p_node_id++))>=0){
                for(int k=0;k<l_size;k++){
                    score=values[i*l_size+j]+ll_weights[k*l_size+j];
                    for(int n=0;n<nb;n++){
                        if(alphas[(node_id*l_size+k)*nb+n].node_id==-2)continue;
                        Alpha_Beta tmp={score+alphas[(node_id*l_size+k)*nb+n].value,
                                node_id,k*nb+n};
                        //printf("add, %d %d %d %d %d\n",i,j,node_id,k,n);
                        nb_heap_insert(alphas+(i*l_size+j)*nb,nb,count,tmp);
                    }
                }
            }
            
            if((nodes[i].type==1)||(nodes[i].type==3)){
                Alpha_Beta tmp={values[i*l_size+j],-1,0};
                nb_heap_insert(alphas+(i*l_size+j)*nb,nb,count,tmp);
            }
            /*
            printf("(position=%d, label=%d)\n",i,j);
            for(int n=0;n<count;n++){
                printf("value=%d pre_position=%d pre_label=%d\n",alphas[(i*l_size+j)*nb+n].value,
                       alphas[(i*l_size+j)*nb+n].node_id,
                       alphas[(i*l_size+j)*nb+n].label_id/nb);
            }
            getchar();*/
            if(nodes[i].type>=2){
                for(int n=0;n<nb;n++){
                    Alpha_Beta tmp={alphas[(i*l_size+j)*nb+n].value,
                                i,j*nb+n};
                    nb_heap_insert(best,nb,best_count,tmp);
                }
            }
        }
    }
    qsort(best,nb,sizeof(Alpha_Beta),alpha_beta_comp);
    //find the path and label the nodes of it.
    
    //for(node_id=0;node_id<node_count;node_id++)
    //    result[node_id]=-1;
    
    for(int n=0;n<nb;n++){
        Alpha_Beta* tmp=&best[n];
        while(tmp->node_id>=0){
            result[n*node_count+tmp->node_id]=(tmp->label_id)/nb;
            tmp=&(alphas[(tmp->node_id)*l_size*nb+(tmp->label_id)]);
        }
    }
    return;
};

#endif
