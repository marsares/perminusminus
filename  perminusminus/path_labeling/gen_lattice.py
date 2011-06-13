import json
'''
format of the json file
graph::= node *
node ::= [type,[pre],label,[feature]]
'''
def gen_lex():
    s=set()
    for line in open('msr_training.utf8',encoding='utf8'):
        line=line.split()
        if not line or max(len(w)for w in line)>4:continue
        s.update(line)
    for line in open('msr_test_gold.utf8',encoding='utf8'):
        line=line.split()
        if not line or max(len(w)for w in line)>4:continue
        s.update(line)
        
    file=open('lexicon.txt','w',encoding='utf8')
    for w in s:
        print(w,file=file)
    return s


def get_arcs(line):
    arcs=[]
    offset=0
    for w in line:
        arcs.append((offset,offset+len(w)))
        offset+=len(w)
    return arcs


class WID(dict):
    def __call__(self,word):
        if word not in self:
            self[word]=len(self)
        return self[word]

if __name__=='__main__':
    file=open('toy.txt','w',encoding='utf8')
    json_file=open('toy.json','w',encoding='utf8')
    lex=gen_lex()
    print('lex loaded')
    wid=WID()
    for line in open('msr_test_gold.utf8',encoding='utf8'):
        line=line.split()
        if not line or max(len(w)for w in line)>4:continue
        arcs=set(get_arcs(line))
        seq=''.join(line)
        #print(seq)
        start=[[]for i in range(len(seq)+1)]
        end=[[]for i in range(len(seq)+1)]
        nodes=[]
        for i in range(len(seq)):
            for j in range(i+1,min(len(seq)+1,i+1+4)):
                if seq[i:j] in lex:
                    left=seq[i-1] if i>0 else '#'
                    left2=seq[i-2] if i-1>0 else '#'
                    right=seq[j] if j<len(seq) else '#'
                    right2=seq[j+1] if j+1<len(seq) else '#'
                    features=[seq[i:j],'l1'+left,'r1'+right,
                            'l12'+left2+left,'r12'+right+right2]
                    node=[
                        [len(nodes),i,j,[],[]],
                          [(i,j) in arcs,features]
                          ]
                    nodes.append(node)
                    start[i].append(node)
                    end[j].append(node)
        #gen degrees
        for node in nodes:
            i=node[0][1]
            j=node[0][2]
            node[0][3]=[n[0][0] for n in end[i]]
            node[0][4]=[n[0][0] for n in start[j]]
        
        
        for node in nodes:
            t=0
            i=node[0][1]
            j=node[0][2]
            
            if node[0][1]==0:
                t+=1
            if node[0][2]==len(seq):
                t+=2
            node[0][:3]=[t]
            if node[1][0]:
                node[1][0]= 0 if i+1==j else 1
            else:
                node[1][0]=-1
            node[1][1]=[wid(x)for x in node[1][1]]
        
        
        graph_json=[]
        for node in nodes:
            #print(node[0]+node[1])
            node_json=[node[0][0],node[0][1]
                       #,node[0][2]
                    ,node[1][0],node[1][1]]
            
            graph_json.append(node_json)
            
            l=' '.join((str(node[0][0]),' '.join(str(x)for x in node[0][1])
                     ,' '.join(str(x) for x in node[0][2]),'|'
                     ,str(node[1][0]),' '.join(str(x)for x in node[1][1])))
            print(l,file=file)
        print('',file=file)
        print(json.dumps(graph_json),file=json_file)
