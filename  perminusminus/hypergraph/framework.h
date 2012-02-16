#pragma once
#include "hypergraph.h"



namespace daidai{
namespace hypergraph{

/*
 * 特征
 * */
template<typename ValueType,typename NodeData>
class Feature{
public:
    Feature(){};
    virtual int load()=0;
    virtual void add_weights(Hypergraph<ValueType,NodeData>& graph)=0;
};

/*
 * 特征学习（继承特征）
 * */
template<typename ValueType,typename NodeData>
class FeatureLearner: public Feature<ValueType,NodeData>{
public:
    FeatureLearner(){};
    virtual void extract(Hypergraph<ValueType,NodeData>& graph){};
    virtual void make_features()=0;
    virtual void update(Hypergraph<ValueType,NodeData>& graph,int delta,long step){};
    virtual void save(long step){};
};


/*
 * 输入输出
 * */
template<typename ValueType,typename NodeData>
class DataIO{
public:
    virtual int load(Hypergraph<ValueType,NodeData>& graph){};
    virtual int save(Hypergraph<ValueType,NodeData>& graph){};
    virtual void reset(){};
};


/*
 * 解码
 * */
template<typename ValueType,typename NodeData>
class Decoder{
public:
    std::vector<Feature<ValueType,NodeData>* > features;
public:
    void decode(DataIO<ValueType,NodeData>& data_io,DataIO<ValueType,NodeData>& data_output){
        int std=0;
        int rst=0;
        int cor=0;
        Hypergraph<ValueType, NodeData> graph;
        while(data_io.load(graph)){
            decode(graph);
            graph.eval(std,rst,cor);
            data_output.save(graph);
        };
        std::cerr<<std<<" "<<rst<<" "<<cor<<"\n";
    };
    void decode(Hypergraph<ValueType,NodeData>& graph){

        for(size_t i=0;i<features.size();i++){
            //std::cout<<i<<"\n";
            features[i]->add_weights(graph);
        }
        //return;
        graph.forward_algorithm();
    };


};

/*
 * 学习（继承解码）
 * */
template<typename ValueType,typename NodeData>
class Learner : public Decoder<ValueType,NodeData>{
public:
    void learn(DataIO<ValueType,NodeData>& data_io,int iteration,FeatureLearner<ValueType,NodeData>& feature_learner){
        Hypergraph<ValueType,NodeData> graph;
        this->features.push_back(&feature_learner);
        data_io.reset(); 
        while(data_io.load(graph)){
            feature_learner.extract(graph);
        }
        feature_learner.make_features();
        
        long step=0;
        int std;int rst;int cor;
        for(int t=0;t<iteration;t++){
            std::cout<<"iteration: "<<t<<"\n";
            data_io.reset();
            std=rst=cor=0;
            while(data_io.load(graph)){
                step++;
                decode(graph);
                graph.eval(std,rst,cor);
                feature_learner.update(graph,1,step);
            }
            std::cout<<std<<" "<<rst<<" "<<cor<<"\n";
        }
        
        feature_learner.save(step);
        
        data_io.reset();
        std=rst=cor=0;
        while(data_io.load(graph)){
            decode(graph);
            graph.eval(std,rst,cor);
        };
        std::cout<<std<<" "<<rst<<" "<<cor<<"\n";
        double p=(double)cor/rst;
        double r=(double)cor/std;
        double f=2*p*r/(p+r);
        std::cout<<p<<" "<<r<<" "<<f<<"\n";
   }
};

}//end of hypergraph
}//end of daidai
