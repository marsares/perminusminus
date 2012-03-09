#include "lattice.h"
using namespace daidai;
using namespace hypergraph;




int main(){
    
    DataIO<int,LatticeEdge>* io=NULL;
    
    //learning
/*
    io=new LatticeIO("ctb5/training.lattice",'r');
    LatticeFeatureLearner lfl;
    lfl.filename="model.bin";
    
    Learner<int,LatticeEdge> learner;
    learner.learn(*io,20,lfl);

    delete io;
*/

/*
    LatticeFeature lf;
    lf.filename="model.bin";
    lf.load();
    DataIO<int,LatticeEdge>* output=new LatticeIO("output.txt",'w');
    Decoder<int,LatticeEdge> decoder;
    decoder.features.push_back(&lf);
    //test
    io=new LatticeIO("ctb5/test.lattice",'r');
    io->reset();
    decoder.decode(*io,*output);
    */
    //
    //
    std::ifstream ifs("debug.lattice",std::ifstream::in);
    Lattice lattice;
    Graph graph;
    ifs>>lattice;
    lattice_to_graph(lattice,graph);    
    std::cout<<lattice;
     
    std::cout<<"here\n";
    decoder.decode(graph);
    graph_to_lattice(graph,lattice);
    std::cout<<lattice;
    delete io;

}

