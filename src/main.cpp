#include"HtmlEditor.h"
#include<Windows.h>

#define PROMPT "[HtmlEditor]>>"

using namespace std;

int main(){
    SetConsoleOutputCP(CP_UTF8);
    HtmlEditor editor;
    editor.loadSession();
    string cmd;

    cout<<PROMPT;
    while(getline(cin, cmd)){
        if(cmd == "exit"){
            editor.saveAll();
            editor.saveSession();
            break;
        }else{
            editor.handleCommand(cmd);
            cout<<PROMPT;
        }
    }
    return 0;
}