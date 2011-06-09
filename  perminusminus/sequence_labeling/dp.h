#ifndef DY_PRO
#define DY_PRO
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct candidate{int value;int pointer;};

int cand_comp(const void * a, const void * b)
{
    return ( *(int*)b - *(int*)a );
}

void dp_decode(int seq_len
               ,int t_size
               ,int* trans_features
               ,int* output
               ,int** values
               //,int** alphas
               ,int** pointers
               ){
    int tmp;
    for(int i=1;i<seq_len;i++){
        for(int j=0;j<t_size;j++){
            tmp=0;
            for(int k=1;k<t_size;k++){
                if(values[i-1][k]+trans_features[k*t_size+j]
                   >values[i-1][tmp]+trans_features[tmp*t_size+j]){
                    tmp=k;
                }
            }
            values[i][j]+=values[i-1][tmp]
                +trans_features[tmp*t_size+j];
            pointers[i][j]=tmp;
        }
    }
    //
    int max_ind=0;
    int offset=seq_len-1;
    for(int i=0;i<t_size;i++)
        if(values[offset][max_ind]<=values[offset][i])max_ind=i;
    output[offset]=max_ind;
    /*find sequence*/
    while(offset){
        max_ind=pointers[offset][max_ind];
        offset--;
        output[offset]=max_ind;
    }
}


void dp_decode2(int seq_len
               ,int t_size
               ,int* trans_features
               ,int* output
               ,int** values
               ,int** alphas
               ,int** betas
             ){
    for(int j=0;j<t_size;j++)alphas[0][j]=values[0][j];
    for(int j=0;j<t_size;j++)betas[seq_len-1][j]=values[seq_len-1][j];
    //forwards
    int tmp;
    for(int i=1;i<seq_len;i++){
        for(int j=0;j<t_size;j++){
            tmp=0;
            for(int k=1;k<t_size;k++){
                if(alphas[i-1][k]+trans_features[k*t_size+j]>alphas[i-1][tmp]+trans_features[tmp*t_size+j]){
                    tmp=k;
                }
            }
            alphas[i][j]=values[i][j]
                +alphas[i-1][tmp]+trans_features[tmp*t_size+j];
        }
    }
    
    for(int i=seq_len-2;i>=0;i--){
        for(int j=0;j<t_size;j++){
            tmp=0;
            for(int k=1;k<t_size;k++){
                if(betas[i+1][k]+trans_features[j*t_size+k]
                   >betas[i+1][tmp]+trans_features[j*t_size+tmp]){
                    tmp=k;
                }
            }
            betas[i][j]=values[i][j]
                +betas[i+1][tmp]+trans_features[j*t_size+tmp];
        }
    }
    for(int i=0;i<seq_len;i++){
        tmp=0;
        for(int j=1;j<t_size;j++){
            if(alphas[i][j]+betas[i][j]-values[i][j]>
                alphas[i][tmp]+betas[i][tmp]-values[i][tmp]){
                tmp=j;
            }
        }
        output[i]=tmp;
    }
}



void dp_n_best_decode(int seq_len
               ,int t_size
               ,int* trans_features
               ,int* output
               ,int** values
               ,int** pointers
               ,int n_best
               ,candidate* n_best_candidates
               ,int** n_best_outputs
               ){

    int ind=0;
    for(int i=1;i<seq_len;i++){
        for(int j=0;j<t_size;j++){
            ind=0;
            for(int k=0;k<t_size;k++){
                for(int l=0;l<n_best;l++){
                    if(pointers[i-1][k*n_best+l]<0)continue;
                    n_best_candidates[ind].value=values[i-1][k*n_best+l]+trans_features[k*t_size+j];
                    n_best_candidates[ind].pointer=k*n_best+l;
                    ind++;
                }
            }
            qsort(n_best_candidates,ind,sizeof(candidate),cand_comp);
            int value=values[i][j*n_best];
            for(int l=0;l<(n_best>ind?ind:n_best);l++){
                values[i][j*n_best+l]=value+n_best_candidates[l].value;
                pointers[i][j*n_best+l]=n_best_candidates[l].pointer;
                
            }
        }
    }
    ind=0;
    for(int j=0;j<t_size*n_best;j++){
        if(pointers[seq_len-1][j]<0)continue;
        n_best_candidates[ind].value=values[seq_len-1][j];
        n_best_candidates[ind].pointer=j;
        ind++;
    }
    qsort(n_best_candidates,ind,sizeof(candidate),cand_comp);
    ind=(n_best>ind?ind:n_best);
    
    int rank;
    for(rank=0;rank<ind;rank++){
        int pointer=n_best_candidates[rank].pointer;
        int offset=seq_len;
        while(offset){
            offset--;
            n_best_outputs[rank][offset]=pointer/n_best;
            pointer=pointers[offset][pointer];
        }
    }
    
    for(;rank<n_best;rank++)
        n_best_outputs[rank][0]=-1;

    for(int i=0;i<seq_len;i++)
        output[i]=n_best_outputs[0][i];
}


#endif
