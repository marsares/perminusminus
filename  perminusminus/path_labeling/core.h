#ifndef __CORE_H__
#define __CORE_H__



struct Node_Header{
    int len;
    int label;
    int type;
    int in_degree_offset;
    int out_degree_offset;
};


struct Graph_Header{
    int len;// # of the 4*byte
    int size;// # of the nodes
};

#endif
