#ifndef __GRAPH_FILE_H__
#define __GRAPH_FILE_H__
#include<stdio.h>
#include<stdlib.h>
#include"dp.h"

/*
binary file format for each graph
remaining_int_length node_count
(node type)+ (predecessors -1)+ (label)+ (features -1)+
*/
/*
Text file format for each graph
node_count
(node type)+
(predecessors -1)+
(label)+
(features -1)+
*/

struct Graph{
    int node_count;
    Node* nodes;
    int* labels;
    int* features;
    int* buffer;
    Graph(int node_count){
        this->node_count=node_count;
        this->nodes=new Node[node_count];
    };
    ~Graph(){
        delete[] this->nodes;
        if((this->buffer)!=NULL)realloc(this->buffer,0);
    };
};



struct Graph_Loader{
    FILE* pFile;
    int mode;
    Graph** graphs;
    int g_size;
    int g_id;
    int max_g_size;
    Graph_Loader(char* filename,int mode=0){
        this->pFile=fopen(filename,"rb");
        this->mode=mode;
        if(mode==1){
            g_size=0;
            g_id=-1;
            max_g_size=256;
            graphs=NULL;
            graphs=(Graph**)realloc(graphs,sizeof(Graph)*max_g_size);
        }
    };
    void load_graph(int data_size,Graph* &graph){
        int* buffer=(int*)malloc(data_size*4);
        fread(buffer,4,data_size,pFile);
        int* p=buffer;
        int node_count;
        node_count=*(p++);
        graph=new Graph(node_count);
        graph->buffer=buffer;
        for(int i=0;i<graph->node_count;i++){
            graph->nodes[i].type=*(p++);
        }
        for(int i=0;i<graph->node_count;i++){
            graph->nodes[i].predecessors=p;
            while((*(p++))>=0);
        }
        graph->labels=p;
        graph->features=p+node_count;
    }
    
    int load(Graph* &graph){
        if(mode==1){
            if(pFile!=NULL){//read
                int data_size;
                if(!fread(&data_size,sizeof(int),1,this->pFile)){
                    fclose(pFile);
                    g_id=-1;
                    pFile=NULL;
                    graph=NULL;
                    return 0;
                }
                load_graph(data_size,graph);
                g_size++;
                if(g_size==max_g_size){
                    max_g_size*=2;
                    graphs=(Graph**)realloc(graphs,sizeof(Graph)*max_g_size);
                }
                graphs[g_size-1]=graph;
                return 1;
            }else{
                g_id++;
                if(g_id==g_size){
                    graph=NULL;
                    g_id=-1;
                    return 0;
                }
                graph=graphs[g_id];
                return 1;
            }
        }
        delete graph;
        int data_size;
        if(!fread(&data_size,sizeof(int),1,this->pFile)){
            graph=NULL;
            fseek(pFile,0,SEEK_SET);
            return 0;
        }
        load_graph(data_size,graph);
        return 1;
    };
};
#endif
