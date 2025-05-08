#include"CmdParser.h"
#include<iostream>
#include<cassert>

using namespace std;

void testCmdParser(){
    std::cout<< "test command parser......"<<endl;
    CmdParser parser;
    std::string input = "insert body id1 id2 text";
    parser.parse(input);

    auto args = parser.getPositionalArgs();
    assert(args.size() == 5);
    assert(parser.getPositionalArgsByIdx(4) == "text");

    std::cout << "=========== test command parser passed! ==========" << std::endl;
}

int main(){
    testCmdParser();
    return 0;
}