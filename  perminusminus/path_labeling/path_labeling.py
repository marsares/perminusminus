import json

class Model:
    l_size=0
    f_size=0
    ll_weights=None
    fl_weights=None
    
    ave_ll_weights=None#for averaged
    ave_fl_weights=None#for averaged


def gen_graph(filename):
    for line in open("toy.json",encoding='utf8'):
        yield json.loads(line)


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


        

def update(model,graph,labels):
    correct=0
    for i in range(len(graph)):
        if graph[i][-2]!=labels[i]:
            for f_id in graph[i][-1]:
                if labels[i]>=0:
                    model.fl_weights[f_id][labels[i]]-=1
                if graph[i][-2]>=0:
                    model.fl_weights[f_id][graph[i][-2]]+=1
        else:
            correct+=1
    g_cache=-1
    l_cache=-1
    for i in range(len(graph)):
        if graph[i][-2]>=0:
            if g_cache>=0:
                model.ll_weights[g_cache][graph[i][-2]]+=1
            g_cache=graph[i][-2]
        if labels[i]>=0:
            if l_cache>=0:
                model.ll_weights[l_cache][labels[i]]-=1
            l_cache=labels[i]
    return correct
def decode(model,graph):
    values=[[0 for j in range(model.l_size)]for i in range(len(graph))]
    alphas=[[[0,-1] for j in range(model.l_size)]for i in range(len(graph))]
    labels=[-1 for i in range(len(graph))]
    #add values
    for i in range(len(graph)):
        node=graph[i]
        for f_id in node[-1]:
            for j in range(model.l_size):
                pass
                values[i][j]+=model.fl_weights[f_id][j]
            
    #find best
    best_score=[0,-1]
    for i in range(len(graph)):
        node=graph[i]
        #for each node
        for j in range(model.l_size):
            #for each label
            tmp=[0,-1]
            for offset in node[1]:
                p=i+offset
                
                #each precedent node
                for k in range(model.l_size):
                    
                    #each label of the precedent node
                    score=model.ll_weights[k][j]+alphas[p][k][0]
                    if tmp[1]==-1 or score>tmp[0]:
                        tmp=[score,p,k]
            tmp[0]+=values[i][j]
            alphas[i][j]=tmp
            if node[0]>1:#end of a graph
                if best_score[1]==-1 or alphas[i][j][0]>best_score[0]:
                    best_score=[alphas[i][j][0],i,j]
                
    #find path
    
    while True:
        #print(best_score)
        if best_score[1]==-1:break
        labels[best_score[1]]=best_score[2]
        best_score=alphas[best_score[1]][best_score[2]]
    return labels

def learning(model,graphs):
    total,correct=0,0
    for graph in graphs:
        labels=decode(model,graph)#find the best path and lebels
        total+=len(graph)
        correct+=update(model,graph,labels)#update according to the gold standard
    print(total,correct,correct/total)

def train():
    #init the model
    model=Model()
    model.l_size,model.f_size=cal_size(gen_graph('toy.txt'))
    print("Size of labels: {0}, Size of features: {1}".format(
        model.l_size,model.f_size))
    model.fl_weights=[[0 for x in range(model.l_size)]
        for y in range(model.f_size)]
    model.ll_weights=[[0 for x in range(model.l_size)]
        for y in range(model.l_size)]
    
    #begin learning
    for i in range(2):
        print(i)
        learning(model,gen_graph('toy.txt'))


if __name__=='__main__':
    train()
