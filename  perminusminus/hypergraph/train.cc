#include <unistd.h>
#include <cstdlib>
#include <getopt.h>
#include "lattice.h"
#include "extended_features.h"


using namespace daidai;
using namespace hypergraph;

    
int main (int argc,char **argv) {

    int iteration=15;
    static struct option long_options[] =
		{
			{"help",     no_argument,       0, 'h'},
			{"iteration",  required_argument,       0, 'i'},
			{0, 0, 0, 0}
		};
    int c;
    int option_index = 0;
    DAT* sogouw=NULL;
    DAT* sogout=NULL;
    while ( (c = getopt_long(argc, argv, "T:W:i:h",long_options,&option_index)) != -1) {
        switch (c) {
            case 0:
                break;
            case 'W':
                sogouw=new DAT(optarg);
                break;
            case 'T':
                sogout=new DAT(optarg);
                break;
            case 'i' : 
                iteration=atoi(optarg);
                break;
            case 'h' :
            case '?' : 
            default : 

                fprintf(stderr,"");
                return 1;
        }
    }
    if(!(optind+1<argc)){
        fprintf(stderr,"need two auguments for training file and prefix for model files\n");
        return 1;
    }

    DataIO<int,LatticeEdge>* io=NULL;
    
    //learning
    io=new LatticeIO(argv[optind],'r');
    LatticeFeatureLearner lfl;
    lfl.filename=argv[optind+1];
    if(sogouw){
        lfl.node_features.push_back(new DictNodeFeature(sogouw));
    }
    if(sogout){
        lfl.node_features.push_back(new SogouTFeature(sogout));
    }

    Learner<int,LatticeEdge> learner;
    //printf("what\n");
    learner.learn(*io,iteration,lfl);

    delete io;
    delete sogouw;
    delete sogout;
}

