#!/usr/bin/python3
import struct
import sys

def load_trans(model_path='data/model.bin'):
    model_file=open(model_path,'rb')
    l_size,f_size=struct.unpack('ii',model_file.read(8))
    #print(l_size,f_size)

    fmt='i'*l_size
    trans_value=[list(struct.unpack(fmt,model_file.read(4*l_size)))
                for i in range(l_size)]
    #trans_value=[[x/1000 for x in row] for row in trans_value]
    return trans_value

def load_result(result_path='data/result.txt'):
    cache=[]
    for line in open(result_path):
        line=line.split()
        if line:
            cache.append(line)
        else:
            if cache:
                gold=[int(x[0]) for x in cache]
                result=[int(x[1]) for x in cache]
                values=[x[2:]for x in cache]
                for i,row in enumerate(values):
                    values[i]=[[round(float(z)*1000)for z in item.split('/')] for item in row]
                yield gold,result,values
                cache=[]
def load_label_info(label_path='data/label_info.txt'):
    label_info=[]
    label_ind={}
    for line in open(label_path):
        pos,tag=line.strip().split('/')
        pos=int(pos)
        label_ind[(pos,tag)]=len(label_info)
        label_info.append((pos,tag))
        
    return label_info,label_ind

def gold_set(gold):
    g_set=set()
    offset=0
    for i,item in enumerate(gold):
        #print(i,item)
        pos,tag=item
        if pos>=2:
            arc=(offset,i+1,tag)
            g_set.add(arc)
            offset=i+1
    return g_set
    
def check_wf(go,rs):
    l=sorted(list(go))[-1][1]
    rst=sorted(list((a,b) for a,b,t,m in rs if m==0))
    ee=0
    for b,e,*r in rst:
        if b>ee:return False
        ee=e
    return True

if __name__=="__main__":
    
    if len(sys.argv)==1:
        model_path='data/model.bin'
        label_path='data/label_info.txt'
        result_path='data/result.txt'
        threshold=50
    else:
        model_path=sys.argv[1]
        label_path=sys.argv[2]
        result_path=sys.argv[3]
        threshold=int(sys.argv[4])*1000
    
    std,rst,cor=0,0,0
    trans=load_trans(model_path)
    label_info,label_ind=load_label_info(label_path)
    #print(label_info)
    #input()
    md=0
    for gold,result,values in load_result(result_path):
        gold=[label_info[x]for x in gold]
        gs=gold_set(gold)
        #print(gs)
        #input()
        #print(result)
        #print(values)
        
        
        score=max(max(a+b-v for a,v,b in vv) for vv in values)
        
        reduced=[]
        for row in values:
            reduced_row={}
            for i in range(len(row)):
                a,v,b=row[i]
                s=a+b-v
                if s+threshold<score:
                    row[i]=None
                else:
                    reduced_row[label_info[i]]=(a,v,b)
            #print(sum(1 for x in row if x))
            #print((reduced_row))
            reduced.append(reduced_row)
            #input()
        arcs=[]
        for start in range(len(reduced)):
            for li,vs in reduced[start].items():
                #print('>',li)
                if li[0]==3:#single character word
                    margin=score-(vs[0]+vs[2]-vs[1])
                    arc=(start,start+1,li[1],round(margin))
                    #print(arc)
                    arcs.append(arc)
                elif li[0]==0:#beginning of a m-c word
                    end=start+1
                    tag=li[1]
                    value=vs[0]#alpha
                    last_i=0
                    while end<len(reduced):
                        if (2,tag) in reduced[end]:
                            this_score=value+reduced[end][(2,tag)][2]+trans[label_ind[(last_i,tag)]][label_ind[(2,tag)]]
                            margin=score-this_score
                            arc=(start,end+1,tag,round(margin))
                            #print(arc)
                            if margin<=threshold:arcs.append(arc)
                        if (1,tag)not in reduced[end]:break
                        value+=trans[label_ind[(last_i,tag)]][label_ind[(1,tag)]]+reduced[end][(1,tag)][1]
                        last_i=1
                        end+=1
                    pass
        #print(arcs)
        print(' '.join(','.join([str(l),str(r),t,str(m)]) for l,r,t,m in arcs))
        
        '''if not check_wf(gs,arcs):
            print('<<<<<<<<<<<<<<<<<<<<')
            print(sorted(gs))
            print(arcs)
            for x in reduced:
                print(x)
            print(gold)
            
            input()'''
        rs={(l,r,t) for l,r,t,m in arcs}
        
        rs={(l,r) for l,r,t in rs}
        gs={(l,r) for l,r,t in gs}
        cor+=len(gs&rs)
        std+=len(gs)
        rst+=len(rs)
        
    #print(std,rst,cor)
    #print(cor/std)
    #print(md)
