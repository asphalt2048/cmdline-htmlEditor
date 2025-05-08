#include<hunspell/hunspell.hxx>
#include<iostream>
#include<cassert>

using namespace std;


void testSimpleSpellCheck(){
    std::cout<< "testSimpleSpellCheck..."<<endl;
    Hunspell hunspell("./data/en_US.aff", "./data/en_US.dic");
    std::vector<std::string> words = {
        "hello",     // correct
        "world",     // correct
        "computer",  // correct
        "language",  // correct
        "beautiful", // correct
        "helloo",    // wrong
        "worlld",    // wrong
        "cmputer",   // wrong
        "languaage", // wrong
        "beutiful"   // wrong
    };

    for (int i=0; i<words.size(); i++) {
        bool is_correct = hunspell.spell(words[i].c_str());
        if(i<5)
            assert(is_correct == true);
        else
            assert(is_correct == false);
    }
    std::cout << "=========== testSimpleSpellCheck passed! ==========" << std::endl;
}

int main(){
    testSimpleSpellCheck();
    return 0;
}