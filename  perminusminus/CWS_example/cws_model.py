#!/usr/bin/python3
from ctypes import *
import math
import json
import sys
import io
import json_to_binary
import cws_transform
import platform
import time
import indexer

class CWS_Data:
    def __init__(self,index):
        self.fid=indexer.Indexer(index)
    def map_seq(self,seq):
        seq=[x for x in seq]
        for i,c in enumerate(seq):
            o=ord(c)
            if o>32 and o<127:
                seq[i]=chr(o+65248)
        seq=''.join(seq)
        return seq
    def encode(self,seq):
        seq=self.map_seq(seq)
        graph=[]
        fs=[filter(lambda x:x>=0,[self.fid(k) for k in cws_transform.gen_keys(seq,x)]) for x in range(len(seq))]
        
        for v in fs:
            graph.append([0,[],0,v])
        graph[0][0]+=1;
        graph[-1][0]+=2;
        for i in range(1,len(graph)):
            graph[i][1]=[i-1]
        return graph
    def decode(self,seq,tags):
        offset=0
        rst=[]
        for i in range(len(seq)):
            if tags[i]>=2:
                rst.append(seq[offset:i+1])
                offset=i+1
        return rst


class CWS_Model:
    def __init__(self,libfile,indexfile,modelfile):
        self.sdlib = cdll.LoadLibrary(libfile)
        self.p_permm=c_void_p()
        self.sdlib.permm_init(c_char_p(modelfile.encode()),c_int(0),byref(self.p_permm))
    
        self.cws_data=CWS_Data(indexfile)
    
        
    def __call__(self,seq):
        
        if not seq:return ''
        x=POINTER(c_int)()
        
        graph=self.cws_data.encode(seq)
        bs=json_to_binary.graph_to_bytes(graph)
        
        p_graph=c_void_p()
        self.sdlib.permm_parse_graph(c_char_p(bs),byref(p_graph))
        self.sdlib.permm_decode(byref(self.p_permm),byref(p_graph),byref(x))
        
        rst=self.cws_data.decode(seq,x)
        
        return(' '.join(rst))


if __name__=='__main__':
    libfile=''
    if platform.system()=='Linux':
        libfile='./libper--.so'
    else:
        libfile='libper--'
    model=CWS_Model(libfile,'index.txt',"model.bin")
    
    ot=time.time()
    try:
        while True:
            seq=' '.join([model(s) for s in input().split()])
            print(seq)
    except EOFError:
        pass
    except KeyboardInterrupt:
        pass
    else:
        pass
    print(time.time()-ot)
    
