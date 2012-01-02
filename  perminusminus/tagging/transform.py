#!/usr/bin/python3
import json
import sys
import json_to_binary
import indexer
import getopt

tag_map={'X':'FW','VP':'PU','NP':'NN',
        }

def _to_tags(postags,sen,lid):
    tags=[]
    for t,w in zip(postags,sen):
        #t=''#debug
        if len(w)==1:
            tags.append('3/'+t)
        else:
            tags.append('0/'+t)
            for i in range(len(w)-2):tags.append('1/'+t)
            tags.append('2/'+t)
        
        
    tags=[lid(t)for t in tags]
    return tags

def gen_keys(seq,i):
    mid=seq[i]
    left=seq[i-1] if i>0 else '#'
    left2=seq[i-2] if i-1>0 else '#'
    right=seq[i+1] if i+1<len(seq) else '#'
    right2=seq[i+2] if i+2<len(seq) else '#'
    keys=[mid+" 1",left+" 2",right+" 3"]

    keys.append(left+mid+' 1')
    keys.append(mid+right+' 2')
    keys.append(left2+left+' 3')
    keys.append(right+right2+' 4')

    return keys

class DictFeature:
    def __init__(self,file):
        self.d=set()
        self.prefix=set()
        for line in open(file):
            line=line.split()
            if not line:continue
            self.d.add(line[0])
            for i in range(len(line[0])):
                self.prefix.add(line[0][:i])
                #print(len(line[0]),line[0][:i])
            #input()
        
    def __call__(self,seq,inder,fs):
        #print(seq)
        for i in range(len(seq)):
            
            for j in range(i+1,len(seq)):
                #print(seq[i:j])
                if seq[i:j] in self.d:
                    if i+j==1:
                        fs[i].append(inder('dict_S'))
                    else:
                        fs[i].append(inder('dict_B'))
                        fs[j-1].append(inder('dict_E'))
                        for k in range(i+1,j-1):
                            fs[k].append(inder('dict_M'))
                if seq[i:j] not in self.prefix:
                    break
        
        pass

def trans(src,dst,index,label_index,mode='w',sep='/',dictionary=None):
    lid=indexer.Indexer(label_index,mode)
    inder=indexer.Indexer(index,mode)
    if dictionary:dict_feature=DictFeature(dictionary)
    
    
    
    file=open(dst,'wb')
    ln=0
    for line in open(src,encoding='utf8'):
        ln+=1
        wts=[x.rpartition(sep) for x in line.strip().split(' ')]
        if sep==' ':
            tags=['' for x in wts]
            line=[x[-1] for x in wts]
        else:
            tags=[x[-1] for x in wts]
            line=[x[0] for x in wts]
        seq=''.join(line)
        graph=[]
        
        fs=[[inder(k) for k in gen_keys(seq,x)]
                for x in range(len(seq))]
        if dictionary:dict_feature(seq,inder,fs)
        fs=[list(filter(lambda x:x>=0,fv))
                for fv in fs]
        
        
        #print(fs)
        #input()
        for c,v in zip(_to_tags(tags,line,lid),fs):
            graph.append([0,[],c,v])
        if not graph:continue
        graph[0][0]+=1;
        graph[-1][0]+=2;
        for i in range(1,len(graph)):
            graph[i][1]=[i-1]
        
        json_to_binary.graph_to_file(graph,file)

        if ln%1000==0:
            print(ln)
            #if ln>5000:break

    file.close()
    print(len(inder))
    
    print('the end')



if __name__=='__main__':
    """
    参数
    文本文件 二进制文件 特征索引 标注索引
    """
    optlist,args=getopt.gnu_getopt(sys.argv[1:],"lps:d:")
    opts={k:v for k,v in optlist}
    #print(opts)
    #print(args)
    
    mode='w' if '-l' in opts else 'r'
    sep=opts.get('-s','_')
    dictionary=opts.get('-d',None)
    
    trans(args[0],args[1],args[2],args[3],mode,sep,dictionary)
    
