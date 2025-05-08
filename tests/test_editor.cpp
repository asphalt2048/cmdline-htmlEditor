#include<iostream>
#include<cassert>
#include<memory>
#include<vector>
#include<Windows.h>
#include"HtmlEditor.h"

using namespace std;

void testSimpleLoadSession(){
    cout<<"test simeple load session......"<<endl;
    HtmlEditor editor("./tests/test_data/config.json");
    editor.loadSession();
    assert(editor.getActiveFile() == nullptr);
    std::cout << "=========== test simple load session passed! ==========" << std::endl;
}

void testComplexLoadSession(){
    cout<<"test complex load session......"<<endl;
    HtmlEditor editor("./tests/test_data/complexConfig.json");
    editor.loadSession();
    assert(editor.getActiveFile() != nullptr);
    assert(editor.getActiveFile()->getRoot()->getTag() == "html");
    std::cout << "=========== test complex load session passed! ==========" << std::endl;
}

void testComplexSaveSession(){
    cout<<"test complex save session......"<<endl;
    HtmlEditor editor("./tests/test_data/complexConfig.json");
    editor.loadSession();
    bool showId = editor.getActiveFile()->getShowID();
    editor.getActiveFile()->setShowID(!showId);
    editor.saveSession();
    editor.loadSession();
    assert(editor.getActiveFile() != nullptr);
    assert(editor.getActiveFile()->getRoot()->getTag() == "html");
    assert(editor.getActiveFile()->getShowID() == !showId);
    std::cout << "=========== test complex save session passed! ==========" << std::endl;
}

void testLoadFile(){
    cout<<"test load file......"<<endl;
    HtmlEditor editor("./tests/test_data/complexConfig.json");
    editor.loadSession();
    auto doc = editor.getActiveFile();
    auto root = doc->getRoot();
    auto content = root->toHtmlString();
    std::string except = 
    "<html>\n"
    "  <head>\n"
    "  </head>\n"
    "  <body>\n"
    "    <p id=\"p1\">\n"
    "      Hello World\n"
    "    </p>\n"
    "  </body>\n"
    "</html>\n";
    assert(content == except);
    std::cout << "=========== test load file passed! ==========" << std::endl;
}

void testSaveFile(){
    cout<<"test save file......"<<endl;
    HtmlEditor editor("./tests/test_data/complexConfig.json");
    editor.loadSession();
    auto doc = editor.getActiveFile();
    std::string fp = doc->getFilePath();

    ifstream inFile(fp);
    assert(inFile.is_open());
    // TODO?
    std::stringstream buffer;
    buffer << inFile.rdbuf();
    std::string content = buffer.str();
    inFile.close();

    try{
        auto node = new ElementNode("div", "test");
        doc->append("html", node);
        editor.saveCurrent();
        editor.closeCurrent();
        editor.loadFileByPath(fp);
    }catch(std::exception e){
    }
    ofstream outFile(fp);
    outFile << content;
    outFile.close();
    //assert(doc->getRoot()->getChildren().size() == 3);
    assert(doc->findNodeById("test") != nullptr);
    std::cout << "=========== test save file passed! ==========" << std::endl;
}

void testCommands(){
    cout<<"test commands......"<<endl;
    HtmlEditor editor("./tests/test_data/complexConfig.json");
    editor.loadSession();
    vector<string> EditCmds = {"insert div test1 head", "undo", "append div test2 html", "delete test2", "undo", "undo"};
    vector<string> IOcmd = {"load ./tests/test_data/bar.html",  "edit foo.html", "close"};

    for(const auto& cmd : EditCmds){
        editor.handleCommand(cmd);
    }

    for(const auto& cmd : IOcmd){
        editor.handleCommand(cmd);
    }

    ifstream inFile("./tests/test_data/foo.html");
    stringstream buffer;
    buffer << inFile.rdbuf();
    auto content = buffer.str();
    cout<<content;
    string excepted = 
    "<html>\n"
    "  <head>\n"
    "  </head>\n"
    "  <body>\n"
    "    <p id=\"p1\">\n"
    "      Hello World\n"
    "    </p>\n"
    "  </body>\n"
    "</html>\n";

    assert(content == excepted);
    std::cout << "=========== test commands passed! ==========" << std::endl;
}

int main(){
    testSimpleLoadSession();
    testComplexLoadSession();
    testComplexSaveSession();
    testLoadFile();
    testSaveFile();
    testCommands();
    return 0;
}