import json
import sys
import json_to_binary
import indexer



def _to_tags(postags,sen,lid):
    tags=[]
    for t,w in zip(postags,sen):
        if len(w)==1:
            tags.append('3/'+t)
        else:
            tags.append('0/'+t)
            for i in range(len(w)-2):tags.append('1/'+t)
            tags.append('2/'+t)
    
    tags=[lid(t)for t in tags]
    #print(tags)
    return tags
'''

b_key_set=set()
for line in open('data/bigram.txt',encoding='utf8'):
    line=line.split()
    if int(line[1])<15:break
    b_key_set.add(line[0])
'''
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
    '''
    if left+mid in b_key_set:keys.append(left+mid+' 1')
    if mid+right in b_key_set:keys.append(mid+right+' 2')
    if left2+left in b_key_set:keys.append(left2+left+' 3')
    if right+right2 in b_key_set:keys.append(right+right2+' 4')
    '''
    #print(keys)
    return keys


def trans(src,dst,index,label_index,mode='w',sep='/'):
    lid=indexer.Indexer(label_index,mode)
    inder=indexer.Indexer(index,mode)
    file=open(dst,'wb')
    ln=0
    for line in open(src,encoding='utf8'):
        ln+=1
        #print(line)
        
        wts=[x.rpartition(sep) for x in line.strip().split(' ')]
        tags=[x[-1] for x in wts]
        line=[x[0] for x in wts]
        
        #print(len(wts))
        #input()
        seq=''.join(line)
        
        
        graph=[]
        
        fs=[filter(lambda x:x>=0,[inder(k) for k in gen_keys(seq,x)]) for x in range(len(seq))]
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
    if len(sys.argv)==1:#empty argv, do the debug
        trans('data/ctb5_training.txt','data/training.bin','data/index.txt','data/label_info.txt','w',sep='_')
        trans('data/ctb5_test.txt','data/test.bin','data/index.txt','data/label_info.txt','r',sep='_')
        
        exit()
    
