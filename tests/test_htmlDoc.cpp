#include"HtmlDoc.h"
#include<iostream>
#include<cassert>

using namespace std;

void testInit(){
    std::cout<< "test init......"<<endl;
    HtmlDoc* doc = new HtmlDoc();
    doc->init();
    HtmlNode* root = doc->getRoot();
    assert(root->getTag() == "html");
    auto children = root->getChildren();
    assert(children[0]->getTag() == "head");
    assert(children[1]->getTag() == "body");
    assert(children[0]->getChildren().size() == 1);
    delete doc;
    std::cout << "=========== test init passed! ==========" << std::endl;
}

void testIdMap(){
    std::cout<< "test idMap......"<<endl;
    HtmlDoc* doc = new HtmlDoc();
    doc->init();
    ElementNode* html = new ElementNode("html", "html");
    assert(doc->isIdsValid(html) == false);
    delete doc;
    std::cout << "=========== test idMap passed! ==========" << std::endl;
}

void testBasicActions(){
    std::cout<< "test basic actions......"<<endl;
    HtmlDoc* doc = new HtmlDoc();
    doc->init();
    doc->remove("head");
    auto c = doc->getRoot()->getChildren();
    assert(c.size() == 2);
    assert(c[0]->isDeleted() == true);

    ElementNode* p = new ElementNode("p", "test");
    doc->insert("body", p);
    c = doc->getRoot()->getChildren();
    assert(c.size() == 3);
    assert(c[1]->isDeleted() == false);
    assert(c[1]->getID() == "test");
    delete doc;
    std::cout << "=========== test basic actions passed! ==========" << std::endl;
}

int main(){
    testInit();
    testIdMap();
    testBasicActions();
    return 0;
}