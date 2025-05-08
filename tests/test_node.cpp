#include"HtmlNode.h"
#include<iostream>
#include<cassert>

using namespace std;

void testBasicAction(){
    cout<<"Test basic actions......\n";
    ElementNode* html = new ElementNode("html", "html");
    assert(html != nullptr);
    html->setID("test"); 
    assert(html->getID() == "test");
    ElementNode* body = new ElementNode("body", "body");
    html->addChild(body);
    auto children = html->getChildren();
    assert(children.size() == 1);
    assert(children[0]->getID() == "body");
    assert(children[0]->getParent() == html);
    delete html;
    cout << "=========== test basic actions passed! ==========" << endl;
}

void testToString(){
    cout<<"Test toHtmlString......\n";
    ElementNode* html = new ElementNode("html", "html");
    ElementNode* head = new ElementNode("head", "head");
    head->addChild(new ElementNode("title", "title"));
    html->addChild(head);
    html->addChild(new ElementNode("body", "body"));
    std::string content = html->toHtmlString();
    std::string expected =
    "<html id=\"html\">\n"
    "  <head id=\"head\">\n"
    "    <title id=\"title\">\n"
    "    </title>\n"
    "  </head>\n"
    "  <body id=\"body\">\n"
    "  </body>\n"
    "</html>\n";
    assert(content == expected);
    delete html;
    cout << "=========== test toHtmlString passed! ==========" << endl;
}

int main(){
    testBasicAction();
    testToString();
    return 0;
}