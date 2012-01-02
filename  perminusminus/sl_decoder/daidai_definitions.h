#pragma once
#include<vector>

namespace daidai{

typedef int Character;
typedef std::vector<Character> Word;
typedef std::vector<Character> RawSentence;
typedef std::vector<Word> SegmentedSentence;

enum POC{
    POC_B,
    POC_M,
    POC_E,
    POC_S
};

typedef std::vector<POC> POCSequence;
typedef std::vector< std::vector<POC> > POCGraph;


}
