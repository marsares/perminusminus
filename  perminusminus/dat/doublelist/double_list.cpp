#include<stdlib.h>
#include<stdio.h>
struct Double_List{
    int list_len;
    int* lefts;
    int* rights;
    int head;
    int tail;
    Double_List(){
        list_len=1;
        lefts=(int*)calloc(4,list_len);
        rights=(int*)calloc(4,list_len);
        lefts[0]=-2;
        rights[0]=-2;
        head=0;
        tail=0;
    };
    void use(int ind){
        if(lefts[ind]==-2){
            head=rights[ind];
        }else{
            rights[lefts[ind]]=rights[ind];
        }
        if(rights[ind]==-2){
            tail=lefts[ind];
        }else{
            lefts[rights[ind]]=lefts[ind];
        }
        lefts[ind]=-1;
        rights[ind]=-1;
    }
    void extends(){
        lefts=(int*)realloc(lefts,4*list_len*2);
        rights=(int*)realloc(rights,4*list_len*2);
        
        for(int i=0;i<list_len;i++){
            lefts[list_len+i]=list_len+i-1;
            rights[list_len+i]=list_len+i+1;
        }
        rights[list_len*2-1]=-2;
        lefts[list_len]=tail;
        rights[tail]=list_len;
        tail=list_len*2-1;
        
        list_len*=2;
    };
    void alloc(int size,int* offsets){
        //printf("size %d\n",size);
        //for(int i=0;i<size-1;i++)
        //    printf("%d ",offsets[i]);
        //printf("\n");
        int base=head;
        while(1){
            if(size>1)
                while(base+offsets[size-2]>=list_len)
                    extends();
            int flag=true;
            for(int i=0;i<size-1;i++){
                if(rights[base+offsets[i]]==-1){
                    flag=false;
                    break;
                }
            }
            if(flag){
                use(base);
                for(int i=0;i<size-1;i++)use(base+offsets[i]);
                printf("%d\n",base);
                fflush(stdout);
                break;
            }
            if(rights[base]==-2)extends();
            base=rights[base];
        }
    }
    void print(){
        printf("head %d, tail %d\n",head,tail);
        for(int i=0;i<list_len;i++)
            printf("%d ",lefts[i]);
        printf("\n");
        for(int i=0;i<list_len;i++)
            printf("%d ",rights[i]);
        printf("\n");
    }
};

int main(){
    Double_List*list=new Double_List();
    /*
    list->print();
    list->extends();
    list->print();
    list->extends();
    list->print();
    list->extends();
    list->use(7);
    list->print();
    
    list->extends();
    list->print();
    */
    int size=0;
    int* offsets=NULL;
    while(1){
        scanf("%d",&size);
        if(size==0)break;
        //printf("%d\n",size);
        //fflush(stdout);
        
        if(size>1){
            offsets=(int*)realloc(offsets,4*size);
            for(int i=0;i<size-1;i++){
                scanf("%d",offsets+i);
            }
        }
        
        list->alloc(size,offsets);
        //list->print();
    }
    //list->print();
    return 0;
}
