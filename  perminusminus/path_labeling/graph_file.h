#ifndef __GRAPH_FILE_H__
#define __GRAPH_FILE_H__
#include<stdio.h>
#include<stdlib.h>
#include"dp.h"

/*
format for each graph
remaining_int_length node_count
(node type)+ (predecessors -1)+ (successors -1)+ (label)+ (features -1)+
*/


struct Graph{
    int node_count;
    Node* nodes;
    int* labels;
    int* features;
    Graph(int node_count){
        this->node_count=node_count;
        this->nodes=new Node[node_count];
    };
    ~Graph(){
        delete[] this->nodes;
    };
};


struct Graph_Loader{
    FILE* pFile;
    int buffer_size;
    int* buffer;
    int mode;
    Graph_Loader(char* filename,int mode=0){
        this->pFile=fopen(filename,"rb");
        //int data_size;
        this->buffer_size=100;
        buffer=NULL;
        this->mode=mode;
        this->buffer=(int*)realloc(this->buffer,this->buffer_size*4);
    };
    int load(Graph* &graph){
        int data_size;
        if(!fread(&data_size,sizeof(int),1,this->pFile))
            return 0;
        
        this->buffer=(int*)realloc(this->buffer,data_size*4);
        fread(buffer,4,data_size,pFile);
        int* p=buffer;
        int node_count;
        node_count=*(p++);
        graph=new Graph(node_count);

        for(int i=0;i<graph->node_count;i++){
            graph->nodes[i].type=*(p++);
        }
        for(int i=0;i<graph->node_count;i++){
            graph->nodes[i].predecessors=p;
            while((*(p++))>=0);
        }
        for(int i=0;i<graph->node_count;i++){
            graph->nodes[i].successors=p;
            while((*(p++))>=0);
        }
        graph->labels=p;
        graph->features=p+node_count;
        
        
        return 1;
    };
};
#endif
