#include"Command.h"
#include<iostream>
#include<cassert>

using namespace std;

void testCommand(){
    std::cout<< "test simple command......"<<endl;
    HtmlDoc* doc = new HtmlDoc();
    doc->init();
    auto AppendCmd = std::make_unique<AppendCommand>("p", "test", "html", "Hi, test command");
    doc->executeCommand(std::move(AppendCmd));
    auto c = doc->getRoot()->getChildren();
    assert(c[2]->getTag() == "p");
    assert(c[2]->getID() == "test");
    assert(c[2]->getText() == "Hi, test command");
    delete doc;
    std::cout << "=========== test simple command passed! ==========" << std::endl;
}

void testComplexCommand(){
    std::cout<< "testComplexCommand......"<<endl;
    HtmlDoc* doc = new HtmlDoc();
    doc->init();

    auto AppendCmd = std::make_unique<AppendCommand>("p", "test", "html", "Hi, test command");
    auto InsertCmd = std::make_unique<InsertCommand>("p", "test2", "test", "");
    doc->executeCommand(std::move(AppendCmd));
    doc->executeCommand(std::move(InsertCmd));
    auto c = doc->getRoot()->getChildren();

    assert(c[2]->getID() == "test2");

    doc->undo(); doc->undo();
    c = doc->getRoot()->getChildren();
    assert(c.size() == 4);
    assert(c[2]->isDeleted() == true);
    assert(c[3]->isDeleted() == true);
    assert(doc->hasChanges() == false);

    doc->redo(); doc->redo();
    c = doc->getRoot()->getChildren();
    assert(c[2]->isDeleted() == false);
    assert(c[3]->isDeleted() == false);

    auto EditIDcmd = std::make_unique<EditIdCommand>("test2", "test3");
    auto DeleteCmd = std::make_unique<DeleteCommand>("test3");
    doc->executeCommand(std::move(EditIDcmd));
    doc->executeCommand(std::move(DeleteCmd));
    assert(c[2]->isDeleted() == true);

    delete doc;

    std::cout << "=========== test complex command passed! ==========" << std::endl;
}


int main(){
    testCommand();
    testComplexCommand();
    return 0;
}
