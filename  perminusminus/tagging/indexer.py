#!/usr/bin/python3
import sys
import argparse

"""
mode='w' 'r' or 'a', just like a file
"""

class Indexer(dict):
    def __init__(self,filename,mode='r'):
        if mode=='r' or mode=='a':
            for line in open(filename,encoding='utf8'):
                line=line.strip()
                self[line.strip()]=len(self)
        self.append=False
        if mode=='w' or mode=='a':
            self.append=True
            self.file=open(filename,mode,encoding='utf8')
        
            
    def __call__(self,key):
        if self.append:
            if key not in self:
                self[key]=len(self)
                print(key,file=self.file)
                self.file.flush()
            return self[key]
        else:
            return self.get(key,-1)

if __name__=='__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('filename')
    parser.add_argument('-mode',default='r',choices='wra',
            help='choose the mode to open the index file')

    args = parser.parse_args()
    
    indexer=Indexer(args.filename,args.mode)
    
    try:
        while True:
            print(' '.join(str(indexer(key)) for key in input().split()))
    except KeyboardInterrupt:
        exit()
    except EOFError:
        exit()
