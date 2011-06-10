


l_size,f_size=0,0
fl_weights=None
ll_weights=None

def gen_graph(filename):
    cache=[]
    for line in open(filename,encoding='utf8'):
        a,x,b=line=line.partition('|')
        if x!='|':
            if cache:
                yield cache
                cache=[]
            continue
        a=[int(x)for x in a.split()]
        b=[int(x)for x in b.split()]
        
        in_d=[x for x in a[1:]if x<0]
        out_d=[x for x in a[1:]if x>0]
        node=[a[0],in_d,out_d,b[0],b[1:]]
        cache.append(node)


def cal_size(graphs):
    max_l_id=0
    max_f_id=0
    for g in graphs:
        for node in g:
            if max_l_id<node[-2]:max_l_id=node[-2]
        if node[-1]:
            f_id=max(node[-1])
            if max_f_id<f_id:max_f_id=f_id
    return max_l_id+1,max_f_id+1

def learning(graphs):
    for graph in graphs:
        decode(graph)
        '''
        1. find path
        2. update
        '''

def update(graph,path):
    pass
def decode(graph):
    values=[[0 for j in range(l_size)] for i in range(len(graph))]
    alphas=[[(0,-1) for j in range(l_size)] for i in range(len(graph))]
    
    #add values
    
    for i in range(len(graph)):
        node=graph[i]
        #for each node
        for j in range(l_size):
            #for each label
            for p in node[1]:
                #each precedent node
                for k in range(l_size):
                    #each label of the precedent node
                    pass
                
    
    '''
    values for (node, label)
    values for (label, label)
    alphas (value, pointer) for (node, label)
    
    
    # add all the values
    # find path
    '''

if __name__=='__main__':
    l_size,f_size=cal_size(gen_graph('toy.txt'))
    
    print(l_size,f_size)
    
    fl_weights=[[0 for x in range(l_size)]for y in range(f_size)]
    ll_weights=[[0 for x in range(l_size)]for y in range(l_size)]
    learning(gen_graph('toy.txt'))
