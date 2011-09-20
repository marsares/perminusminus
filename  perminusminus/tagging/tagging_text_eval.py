#!/usr/bin/python3

def to_set(seq):
    offset=0
    s=set()
    for w,t in seq:
        #s.add((offset,offset+len(w),t))
        s.add((offset,offset+len(w)))
        offset=offset+len(w)
    return s
if __name__=="__main__":
    std,rst,cor=0,0,0
    for gold,result in zip(open('data/ctb_test.txt'),open('data/ctb_test_result.txt')):
        gold=gold.strip().split(' ')
        gold=[x.split('_') for x in gold]
        result=result.strip().split(' ')
        result=[x.split('/') for x in result]
        gold=to_set(gold)
        result=to_set(result)
        std+=len(gold)
        rst+=len(result)
        cor+=len(gold&result)
    print(std,rst,cor)
    p=cor/rst
    r=cor/std
    print(p,r,2*p*r/(p+r))
