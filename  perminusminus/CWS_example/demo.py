import http.server
import cgi
import urllib.parse
from ctypes import *
import math
import json
import sys
import json_to_binary
import cws_transform
import cws_model
import multiprocessing
import webbrowser
import platform


class CWS_Data:
    def __init__(self,index):
        file=open(index,encoding='utf8')
        self.fid=cws_transform.FeatureIndexer()
        self.fid.update(json.load(file))
        file.close()
    def encode(self,seq):
        graph=[]
        fs=[self.fid.test(cws_transform.gen_keys(seq,x)) for x in range(len(seq))]
        
        for v in fs:
            graph.append([0,[],0,v])
        graph[0][0]+=1;
        graph[-1][0]+=2;
        for i in range(1,len(graph)):
            graph[i][1]=[i-1]
        return graph
    def decode(self,seq,tags):
        offset=0
        rst=[]
        for i in range(len(seq)):
            if tags[i]>=2:
                rst.append(seq[offset:i+1])
                offset=i+1
        return rst


libfile=''
if platform.system()=='Linux':
    libfile='../path_labeling/libper--.so'
else:
    libfile='libper--'
model=cws_model.CWS_Model(libfile,'tmp/index.txt',"tmp/model.bin")

lock=multiprocessing.Lock()
def do_cws(seq):
    if not seq:
        return ''
    
    lock.acquire()
    rst=model(seq)
    lock.release()
    
    return rst


class MyHttpHandler(http.server.BaseHTTPRequestHandler):
    def do_GET(self):
        index=open('index.html',encoding='utf8').read()
        
        path=urllib.parse.unquote(self.path)
        rst=''
        ret=''
        if len(path)>1 and '.ico' not in path:
            path=path[1:]
            
            #print(path)
            txt=path.split('\n')
            #print(txt)
            
            rst='\n'.join((cws(line) if line else line)for line in txt)
            
            #print(rst)
        else:
            path=''
        #print(self.path)
        #print('hi')
        
        ret=index.replace('输入中文',path).replace('在这里显示分词结果',rst)
        
        
        self.send_response( 200 )
        self.send_header( "Content-type", "text/html" )
        self.send_header('Set-Cookie','user=190')
        self.end_headers()
        
        self.wfile.write(ret.encode('utf8'))
    

def run(server_class=http.server.HTTPServer, handler_class=http.server.BaseHTTPRequestHandler):
    server_address = ('', 8000)
    httpd = server_class(server_address, handler_class)
    httpd.serve_forever()
    return httpd;

def cws(s):
    return ' '.join(do_cws(x) for x in s.split())

print('server started')
url = "http://localhost:8000/"
webbrowser.open(url,new=2)
run(handler_class=MyHttpHandler)

