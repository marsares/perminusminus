import sys
def to_set(seq,labels,with_tag):
    offset=0
    s=set()
    #print(seq)
    seq=[int(x)for x in seq]
    for i,label_ind in enumerate(seq):
        x,tag=(labels[label_ind])
        if x=='2' or x=='3':
        #if '2' in labels[label_ind] or '3' in labels[label_ind]:
            if with_tag:
                s.add((offset,i+1,tag))
            else:
                s.add((offset,i+1))
            offset=i+1
            
    if (i+1)!=offset:
        s.add((offset,i+1))
    #print(s)
    #input()
    
    return s
def eval_sen(g,r,labels,with_tag):
    
    go=to_set(g,labels,with_tag)
    re=to_set(r,labels,with_tag)
    co=go&re
    
    
    #print(re)
    #input()
    return len(go),len(re),len(co)


def one_pass(with_tag=True):
    labels=[line.strip().split('/') for line in open('data/label_info.txt')]
    
        
    filename='data/result.txt'
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
                a,b,c=eval_sen(g,r,labels,with_tag)
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

if __name__=='__main__':
    one_pass(False)
    one_pass(True)
