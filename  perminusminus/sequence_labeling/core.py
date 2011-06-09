import json
import time

def decode(values,trans):
    _tag_set_size=len(values[0])
    pointers=[[0 for i in range(_tag_set_size)]for j in range(len(values))]
    for i in range(1,len(values)):
        for j in range(_tag_set_size):
            tmp=None
            v,k=max([(trans[k][j]+values[i-1][k],k)for k in range(_tag_set_size)])
            values[i][j]+=v
            pointers[i][j]=k
    #'''find max'''
    v,i=max((values[-1][i],i) for i in range(len(values[-1])))
    #'''find sequence'''
    max_seq=[0 for j in range(len(pointers))]
    offset=len(max_seq)-1
    max_seq[offset]=i
    while offset>0:
        i=pointers[offset][i]
        offset-=1
        max_seq[offset]=i
    return max_seq



def gen_sentences(file,record=[0,0]):
        '''load a file as the training set'''
        cache=[[],[]]
        max_tag=0
        max_f=0
        for line in open(file,encoding='utf8'):
            line=line.split()
            if len(line)==0:
                if cache[0]:
                    yield cache
                    cache=[[],[]]
            else:
                tag=int(line[0])
                f=[int(x) for x in line[1:]]
                max_tag=max(max_tag,tag)
                max_f=max(max_f,max(f))
                cache[1].append(f)
                cache[0].append(tag)
        record[0]=max_tag+1
        record[1]=max_f+1
        if cache[0]:
            yield cache
        
        

def train(training_file,model_file,n=20):
    '''
    train a model
    '''
    def update():
        if all(ystar==y for ystar,y in zip(ystars,ys)):return
        for i in range(len(ystars)):
            ystar,y=ystars[i],ys[i]
            if ystar==y:continue
            for fid in fs[i]:
                features[fid][y]-=1
                features[fid][ystar]+=1
                ave_features[fid][y]-=stop_step-step
                ave_features[fid][ystar]+=stop_step-step
                
        for i in range(len(ystars)-1):
            if (ystars[i],ystars[i+1])==(ys[i],ys[i+1]):continue
            trans[ys[i]][ys[i+1]]-=1
            trans[ystars[i]][ystars[i+1]]+=1
            ave_trans[ys[i]][ys[i+1]]-=stop_step-step
            ave_trans[ystars[i]][ystars[i+1]]+=stop_step-step
        
    #load
    rec=[0,0]
    cor_len=0
    for s in gen_sentences(training_file,rec):
        cor_len+=1
    
    tag_size,f_size=rec
    #sentences,tag_size,f_size=load(training_file)
    
    #data storage for model
    features=[[0 for i in range(tag_size)] for j in range(f_size)]
    ave_features=[[0 for i in range(tag_size)] for j in range(f_size)]
    trans=[[0 for i in range(tag_size)]for j in range(tag_size)]
    ave_trans=[[0 for i in range(tag_size)]for j in range(tag_size)]
    
    
    #learning
    stop_step=n*cor_len
    step=-1
    for pass_id in range(n):
        print('pass',pass_id)
        tokens,correct=0,0
        otime=time.time()
        for tags,fs in gen_sentences(training_file,rec):
            step+=1
            if len(tags)==0:continue#empty sentence
            #fill values
            values=[[0 for i in range(tag_size)] for j in range(len(tags))]
            for i in range(len(tags)):
                for fid in fs[i]:
                    fv=features[fid]
                    for j in range(tag_size):
                        values[i][j]+=fv[j]
            ystars=tags
            ys=decode(values,trans)#decodeprint()
            
            update()#update
            tokens+=len(values)
            correct+=sum(1 for a,b in zip(ystars,ys) if a==b)
        
        print((time.time()-otime)/60,'min.')
        print('correct/tokens',correct/tokens)
    #save
    file=open(model_file,'w',encoding='utf8')
    json.dump([ave_trans,ave_features],file,ensure_ascii=False,indent=1)
    file.close()

def test(model_file,test_file,result_file):
    file=open(model_file,encoding='utf8')
    [trans,features]=json.load(file)
    tag_size=len(trans)
    file.close()
    print('model loaded')
    file=open(result_file,'w',encoding='utf8')
    
    for tags,fs in gen_sentences(test_file):
        if len(tags)==0:continue#empty sentence
        values=[[0 for i in range(tag_size)] for j in range(len(tags))]
        for i in range(len(tags)):
            for fid in fs[i]:
                fv=features[fid]
                for j in range(tag_size):
                    values[i][j]+=fv[j]
        ystars=tags
        ys=decode(values,trans)#decodeprint()
        print(*[str(a)+' '+str(b) for a,b in zip(ystars,ys)],sep='\n',file=file)
        print('',file=file)
    file.close()
    
if __name__=='__main__':
    train('train.txt','model.txt',n=5)
    #train('toy.txt','model.txt',n=5)
    #test('model.txt','toy.txt','result.txt')