#!/usr/bin/python3
import json
import sys
import json_to_binary
'''
将msr的训练集、测试集特征化序列化，变成工具包可以处理的样子
'''

class FeatureIndexer(dict):
    def __init__(self):
        pass
    def __call__(self,keys):
        return [self.setdefault(k,len(self))for k in keys]
    def test(self,keys):
        return [self[k] for k in keys if k in self]
        
def _to_tags(sen):
    tags=[]
    for w in sen:
        if len(w)==1:
            tags.append(3)
        else:
            tags.append(0)
            for i in range(len(w)-2):tags.append(1)
            tags.append(2)
    return tags


def gen_keys(seq,i):
    mid=seq[i]
    left=seq[i-1] if i>0 else '#'
    left2=seq[i-2] if i-1>0 else '#'
    right=seq[i+1] if i+1<len(seq) else '#'
    right2=seq[i+2] if i+2<len(seq) else '#'
    return [mid+":m",left+":l",right+":r",
            left+mid+':L',mid+right+':R',
            left2+left+':2L',right+right2+':2R']

def train(src,dst,index):
    fid=FeatureIndexer()
    file=open(dst,'wb')
    for line in open(src,encoding='utf8'):
        line=line.split()
        seq=''.join(line)
        graph=[]
        fs=[fid(gen_keys(seq,x)) for x in range(len(seq))]
        for c,v in zip(_to_tags(line),fs):
            graph.append([0,[],c,v])
        if not graph:continue
        graph[0][0]+=1;
        graph[-1][0]+=2;
        for i in range(1,len(graph)):
            graph[i][1]=[i-1]
        #print(json.dumps(graph),file=file)
        json_to_binary.graph_to_file(graph,file)
    
    file.close()
    print(len(set(fid.values())))
    print(len(fid))
    
    
    file=open(index,'w',encoding='utf8')
    json.dump(dict(fid),file,ensure_ascii=False,indent=1)
    file.close()
    print('the end')

def test(index,src,dst):
    file=open(index,encoding='utf8')
    fid=FeatureIndexer()
    fid.update(json.load(file))
    file.close()
    file=open(dst,'wb')
    for line in open(src,encoding='utf8'):
        line=line.split()
        seq=''.join(line)
        graph=[]
        fs=[fid.test(gen_keys(seq,x)) for x in range(len(seq))]
        for c,v in zip(_to_tags(line),fs):
            graph.append([0,[],c,v])
        if not graph:continue
        graph[0][0]+=1;
        graph[-1][0]+=2;
        for i in range(1,len(graph)):
            graph[i][1]=[i-1]
        #print(json.dumps(graph),file=file)
        json_to_binary.graph_to_file(graph,file)
    print('the end')
    file.close()

if __name__=='__main__':
    if len(sys.argv)==1:#empty argv, do the debug
        print("reading training data")
        #train('msr_training.utf8','training.bin','index.json')
        print("reading test data")
        #test('index.json','msr_test_gold.utf8','test.bin')
        exit()
    
    task=sys.argv[1]
    if task=='l':
        print("reading training data")
        print((sys.argv[2],sys.argv[3],sys.argv[4]))
        train(sys.argv[2],sys.argv[3],sys.argv[4])
        exit()
    if task=='p':
        print("reading test data")
        test(sys.argv[2],sys.argv[3],sys.argv[4])
        exit()
    
