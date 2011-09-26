#ifndef __UTF8_H__
#define __UTF8_H__

int utf8_stdin_readline(int*&cache,int max_length,int&length){
    length=0;
    int c;
    while(1){//反复读取输入流直到文件末尾或者\n
        if(length==max_length)return -2;
        c=getchar();
        if(c==EOF){
            return c;
        }
        if(!(c&0x80)){//1个byte的utf-8编码
            if(c==10){//回车
                return c;
            }else{//一般ascii字符
                cache[length++]=c;//放入缓存
            }
        }else if(!(c&0x20)){//2个byte的utf-8编码
            cache[length++]=((c&0x1f)<<6)|
                (getchar()&0x3f);
        }else if(!(c&0x10)){//3个byte的utf-8编码
            cache[length++]=((c&0x0f)<<12)|
                ((getchar()&0x3f)<<6)|
                (getchar()&0x3f);
        }else{
            return -3;
        }
    }
}
void utf8_stdout_write(int*sequence,int len){
    int c;
    for(int i=0;i<len;i++){
        c=sequence[i];
        if(c<128){//1个byte的utf-8
            putchar(c);
        }else if(c<0x800){//2个byte的utf-8
            putchar(0xc0|(c>>6));
            putchar(0x80|(c&0x3f));
        }else{//3个byte的utf-8
            putchar(0xe0|((c>>12)&0x0f));
            putchar(0x80|((c>>6)&0x3f));
            putchar(0x80|(c&0x3f));
        }
    }
}
#endif
