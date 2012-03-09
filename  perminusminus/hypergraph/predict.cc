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
			{0, 0, 0, 0}
		};
    int c;
    int option_index = 0;
    DAT* sogouw=NULL;
    while ( (c = getopt_long(argc, argv, "W:h",long_options,&option_index)) != -1) {
        switch (c) {
            case 0:
                break;
            case 'W':
                sogouw=new DAT(optarg);
                break;
            case 'h' :
            case '?' : 
            default : 

                fprintf(stderr,"");
                return 1;
        }
    };
    DataIO<int,LatticeEdge>* io=NULL;
    
   
    LatticeFeature lf;
    
    if(sogouw){
        lf.node_features.push_back(new DictNodeFeature(sogouw));
    }
    lf.filename=argv[optind];
    lf.load();
    DataIO<int,LatticeEdge>* output=new LatticeIO(argv[optind+2],'w');
    Decoder<int,LatticeEdge> decoder;
    decoder.features.push_back(&lf);
    //test
    io=new LatticeIO(argv[optind+1],'r');
    io->reset();
    decoder.decode(*io,*output);
    delete output;

}

