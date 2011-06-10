#ifndef __PATH_LABELING__
#define __PATH_LABELING__

/*the structure of the model, also the file of the model*/
struct Model{
    int model_type;//path, sequence, or point?
    int l_size;//size of the labels
    int f_size;//size of the features
    int* ll_weights;
    int* fl_weights;//weights of (feature,label)
    
    int* ave_ll_weights;
    int* ave_fl_weights;//weights of (feature,label)
};

void save_model(Model* model,char* filename){
}
void load_model(char* filename){
}

#endif
