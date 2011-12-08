#!/usr/bin/python3
import array

if __name__=="__main__":
    labels={}
    for ind,line in enumerate(open('data/label_info.txt')):
        #print(line)
        labels[line.strip()]=[ind,[],[]]
    
    
    for k,v in labels.items():
        if k[0]=='0' or k[0]=='3':
            v[1].extend([i[0] for t,i in labels.items() if t[0]=='3' or t[0]=='2'])
        if k[0]=='1' or k[0]=='2':
            if '0'+k[1:] in labels:v[1].append(labels['0'+k[1:]][0])
            if '1'+k[1:] in labels:v[1].append(labels['1'+k[1:]][0])
        
        
        if k[0]=='2' or k[0]=='3':
            v[2].extend([i[0] for t,i in labels.items() if t[0]=='0' or t[0]=='3'])
        if k[0]=='0' or k[0]=='1':
            if '1'+k[1:] in labels:v[2].append(labels['1'+k[1:]][0])
            if '2'+k[1:] in labels:v[2].append(labels['2'+k[1:]][0])
        print(len(v[1]),len(v[2]))
    labels=[[ind,sorted(pre),sorted(post)]for ind,pre,post in labels.values()]
    labels=sorted(labels)
    #print(labels)
    mem=[len(labels)]
    for i,data in enumerate(labels):
        ind,pre,post=data
        #if(i!=ind):
        #    print(i,ind)
        assert(i==ind)
        mem.extend(pre+[-1])
        mem.extend(post+[-1])
    #mem=[len(mem)]+mem
    print(len(mem))
    
    binfile=open('data/label_trans.bin','wb')
    arr=array.array('i')
    arr.fromlist(mem)
    arr.tofile(binfile)
    binfile.close()
