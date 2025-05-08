#include"Visitor.h"
#include<iostream>
#include<Windows.h>

using namespace std;

void testPrintTree(){
    std::cout<< "test PrintTree..."<<endl;
    ElementNode* html = new ElementNode("html", "html");
    ElementNode* head = new ElementNode("head", "head");
    head->addChild(new ElementNode("title", "title"));
    html->addChild(head);
    html->addChild(new ElementNode("body", "body"));
    PrintTreeVisitor pt(true);
    html->accept(pt);
    std::cout << "=========== test PrintTree passed! ==========" << std::endl;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    testPrintTree();
    return 0;
}

