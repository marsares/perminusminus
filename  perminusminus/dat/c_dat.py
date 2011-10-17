#!/usr/bin/python3
import time
import subprocess
import array
import sys
import os
class DAT:
    """ Class doc """
    
    def __init__ (self):
        #we maintain a two-direction chain
        #the indexes are negatived
        self.base=[+1]
        self.check=[-1]
        self.begin=0
        self.end=0
        exe_file=os.path.join(sys.path[0],"doublelist/double_list")
        self.sp=subprocess.Popen(exe_file,stdin=subprocess.PIPE,stdout=subprocess.PIPE)
    def __del__(self):
        self.sp.stdin.write(("0 ").encode())
        self.sp.stdin.flush()
    def thrink(self):
        end=len(self.check)-1
        while self.check[end]<0:
            end-=1
        size=end+1
        self.base=self.base[:size]
        self.check=self.check[:size]
    def extends(self):
        """extends the chain by doubling the size"""
        l=len(self.check)
        new_check=[-l-x-1 for x in range(l)]
        new_base=[-l-x+1 for x in range(l)]
        
        self.base.extend(new_base)
        self.check.extend(new_check)

    
    def alloc(self,offsets,init_check):#offsets[0]==0
        size=len(offsets)
        self.sp.stdin.write((str(size)+" ").encode())
        for x in offsets[1:]:
            self.sp.stdin.write((str(x)+" ").encode())
        self.sp.stdin.flush()
        x=self.sp.stdout.readline().strip()
        base=int(x.decode())
        
        """alloc the space"""
        while base+offsets[-1]>=len(self.check):
            self.extends()
        
        for o in offsets:
            ind=base+o
            self.base[ind]=0
            self.check[ind]=init_check
        return base
        
    def __call__(self,prefix):
        ind=0
        base=0
        for i,c in enumerate(prefix):
            ind=self.base[ind]+c
            if ind>=len(self.check) or self.check[ind]!=base:
                return -1
            base=ind
        return base
    def get_info(self,prefix):
        ind=0
        base=0
        for i,c in enumerate(prefix):
            ind=self.base[ind]+c
            if ind>=len(self.check) or self.check[ind]!=base:
                return i
            base=ind
        return -base


    def gen_children(self,lex,start,prefix):
        children=set()
        l=len(prefix)
        for ind in range(start,len(lex)):
            word=lex[ind]
            if len(word)<l:break
            if not all(a==b for a,b in zip(word,prefix)):
                break
            if len(word)>l:
                children.add(word[l])
        children=sorted(list(children))
        return children
    
    def add_lex(self,lex):
        lex=sorted(lex)
        size=len(lex)
        print(size)
        #inital the root node
        children=[0]+self.gen_children(lex,0,[])
        base=self.alloc(children,0)
        
        #other nodes
        for i,word in enumerate(lex):
            off=self.get_info(word)
            if off<=0:continue
            for offset in range(off,len(word)):
                prefix=word[:offset]
                p_base=-self.get_info(prefix)
                
                children=self.gen_children(lex,i,prefix)
                
                b=children[0]
                base=self.alloc([x-b for x in children],p_base)
                self.base[p_base]=base-b
            if i%10000==0:
                print(i/size)
    def to_bin(self,bin_file='dat.bin'):
        file=open(bin_file,'wb')
        arr=array.array("i")
        arr.fromlist(self.base)
        arr.fromlist(self.check)
        arr.tofile(file)

def str_ord(s):
    
    return [ord(c)for c in s]


def make_dat(src,dst,sep=None):
    filename=src
    if sep==None:
        lex=[str_ord(line[:-1]) for line in open(filename,encoding="utf8")]
    else:
        lex=[str_ord(line.rpartition(sep)[0]) for line in open(filename,encoding="utf8")]
    dat=DAT()
    otime=time.time()
    dat.add_lex(lex)
    print("time spend: ",(time.time()-otime)/60)
    dat.thrink()
    
    

    
    ind=0
    for line in open(filename,encoding='utf8'):
        word=line[:-1]
        if sep==None:
            value=ind
        else:
            word,x,value=word.rpartition(sep)
            value=int(value)
        #print(len(word),word)
        base=dat(str_ord(word))
        
        dat.base[base]=value
        
        assert(base>0)
        ind+=1
    dat.to_bin(dst)


if __name__=='__main__':
    """
    make sure that there are no a, b in lexicion that:
        a is a prefix of b
    """
    
    sep=None
    if len(sys.argv)>=4:
        sep=sys.argv[3]
    
    make_dat(sys.argv[1],sys.argv[2],sep)
    
