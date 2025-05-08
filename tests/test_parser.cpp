#include"HtmlParserImple.h"
#include<iostream>
#include<cassert>


using namespace std;


void testParse() {
    std::cout<< "testParse..."<<endl;
    GumboParserImple parser;
    HtmlNode* root = parser.parse("./tests/test_data/foo.html");

    assert(root != nullptr); 
    assert(root->getTag() == "html");
    
    auto children = root->getChildren();
    assert(children.size() == 2);
    assert(children[0]->getTag() == "head");

    std::cout << "=========== testParse passed! ==========" << std::endl;

    delete root;
}

int main() {
    //SetConsoleOutputCP(CP_UTF8);
    testParse();
    return 0;
}