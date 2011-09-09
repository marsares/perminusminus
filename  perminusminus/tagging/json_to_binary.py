import json
import array
import struct

'''

type:
    0: mormal
    1: initial
    2: terminal
    3: initial and terminal

format of the json file:
each line is a graph object
graph::= [node, ... ]
node ::= [type,[predecessor, ...],label,[feature, ...]]

format for each graph
remaining_int_length node_count
(node type)+ (predecessors -1)+ (label)+ (features -1)+
'''

def graph_to_file(graph,binfile):
    
    node_count=len(graph)
    node_types=[n[0] for n in graph]
    labels=[n[2] for n in graph]
    #print(labels)
    pre=[]
    features=[]
    for ind,node in enumerate(graph):
        pre.extend([x for x in node[1]])
        pre.append(-1)
        features.extend(node[3])
        features.append(-1)
    arr=array.array('i')
    data=[node_count]+node_types+pre+labels+features
    arr.fromlist([(len(data))]+data)
    arr.tofile(binfile)

def graph_to_bytes(graph):
    
    node_count=len(graph)
    node_types=[n[0] for n in graph]
    labels=[n[2] for n in graph]
    #print(labels)
    pre=[]
    features=[]
    for ind,node in enumerate(graph):
        pre.extend([x for x in node[1]])
        pre.append(-1)
        features.extend(node[3])
        features.append(-1)
    arr=array.array('i')
    data=[node_count]+node_types+pre+labels+features
    arr.fromlist(data)
    #print(data)
    return arr.tobytes()

def transform(json_file,bin_file):
    sen=0
    binfile=open(bin_file,'wb')
    for line in open(json_file,encoding='utf8'):
        sen+=1
        graph=json.loads(line)
        graph_to_file(graph,bin_file)
    binfile.close()
    print("finished",sen,"sentences")

if __name__=='__main__':

    #transform("toy.json","toy.bin")
    #transform("test.json","test.bin")
    transform("training.json","training.bin")
    