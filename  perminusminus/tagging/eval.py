#!/usr/bin/python3
import sys
def to_set(seq):
    offset=0
    s=set()
    #print(seq)
    for i,x in enumerate(seq):
        if x=='2' or x=='3':
            s.add((offset,i+1))
            #print((offset,i+1))
            offset=i+1
    if (i+1)!=offset:
        s.add((offset,i+1))
    return s
def eval_sen(g,r):
    go=to_set(g)
    re=to_set(r)
    co=go&re
    return len(go),len(re),len(co)

if __name__=='__main__':
    filename='result.txt'
    if len(sys.argv)>=2:
        filename=sys.argv[1]
    cache=[]
    std,rst,cor=0,0,0
    for line in open(filename,encoding='utf8'):
        line=line.split()
        
        if not line:
            if cache:
                g=[x[0]for x in cache]
                r=[x[1]for x in cache]
                a,b,c=eval_sen(g,r)
                std+=a
                rst+=b
                cor+=c
                cache=[]
        else:
            cache.append(line)
    if cache:
        g=[x[0]for x in cache]
        r=[x[1]for x in cache]
        a,b,c=eval_sen(g,r)
        std+=a
        rst+=b
        cor+=c
    print(std,rst,cor)
    p=cor/rst
    r=cor/std
    print("precision: {0}, recall: {1}, F1: {2}".format(p,r,2*p*r/(p+r)))
