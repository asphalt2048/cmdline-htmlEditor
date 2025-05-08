#include"HtmlEditor.h"
#include<fstream>
#include<filesystem>
#include<iostream>
#include<nlohmann/json.hpp>

#define COLOR_RED "\033[21;31m"
#define COLOR_END "\033[0m"

namespace fs = std::filesystem;
using json = nlohmann::json;

bool HtmlEditor::askUser(const std::string& descrip, const std::string& ques) const{
    std::string ans;
        while(true){
            std::cout<<descrip<<'\n';
            std::cout<<ques<<"[yes/no]";
            std::cin>>ans;
            if(ans == "yes"){
                return true;
            }
            else if(ans == "no")
                return false;
            else
                continue;
        }
}

int HtmlEditor::findDocByPath(const std::string& path) const{
    for(int i=0; i<HtmlList.size(); i++){
        if(HtmlList[i]->getFilePath() == path)
            return i;
    }
    return -1;
}

int HtmlEditor::findDocByName(const std::string& filename) const{
    fs::path target = filename;
    int i;
    for(i=0; i<HtmlList.size(); i++){
        fs::path fpath = HtmlList[i]->getFilePath();
        if(fpath.filename().string() == target.filename().string()){
            return i;
        }
    }
    return -1;
}

void HtmlEditor::build(const std::string& filename){
    fs::path fpath = filename;
    if(!askUser("This act will create a new file.", "Are you sure you wish to proceed?"))
        return;
    fpath = "./data/" + fpath.filename().string();
    std::cout<<"Initializing a new file, with default save path: " + fpath.string() + "\n";
    if(fs::exists(fpath)){
        std::cout<<"Create file failed: file already exists\n";
        return;
    }
    HtmlDoc* doc = new HtmlDoc();
    doc->setFilePath(fpath.string());
    doc->init();
    try{
        doc->save();
    }catch(std::exception e){
        delete doc;
        std::cout<<"Create file failed\n";
        std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
        return;
    }
    std::cout<<"Initialized a new file, with default save path: " + fpath.string() + "\n";
    HtmlList.push_back(doc);
    activeIndex = (int)HtmlList.size() - 1; 
}

void HtmlEditor::loadFileByPath(const std::string& path){
    fs::path fpath = path;
    if(!fs::exists(fpath)){
        build(fpath.filename().string());
    }
    int pre = activeIndex;
    activeIndex = findDocByPath(path);
    if(activeIndex == -1){
        HtmlDoc* doc = new HtmlDoc();
        doc->setFilePath(path);
        try{
            doc->load(*htmlParser);
            HtmlList.push_back(doc);
            activeIndex = (int)HtmlList.size() - 1;
            return;
        }catch(std::exception e){
            std::cout<<"Load new file failed\n";
            std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
            activeIndex = pre;
        }
    }else{
        switchFileByName(fpath.filename().string());
    }
}

void HtmlEditor::switchFileByName(const std::string& filename){
    int i = findDocByName(filename);
    if(i == -1){
        std::cout<<"Switch file failed: no target found\n";
        return;
    }
    int pre = activeIndex;
    activeIndex = i;
    if(this->getActiveFile()->needParse()){
        try{
            this->getActiveFile()->load(*htmlParser);
        }catch(std::exception e){
            activeIndex = pre;
            std::cout<<"Parse file: " + HtmlList[i]->getFilePath() + " failed";
            std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
        }
    }
}

void HtmlEditor::saveFileByPath(const std::string& filename){
    fs::path fn = filename;
    int i = findDocByName(fn.filename().string());
    if(i == -1){
        std::cout<<"No target found for saving\n";
        return;
    }else{
        try{
            HtmlList[i]->save();
            return;
        }catch(std::exception e){
            std::cout<<"Save file failed\n";
            std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
            return;
        }
    }
}

void HtmlEditor::saveCurrent(){
    try{
        this->getActiveFile()->save();
    }catch(std::exception e){
        std::cout<<"Save file failed\n";
        std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
    }
}

void HtmlEditor::closeCurrent(){
    HtmlDoc* doc = this->getActiveFile();
    delete doc;
    HtmlList.erase(HtmlList.begin() + activeIndex);
    if(HtmlList.empty())
        activeIndex = -1;
    else
        activeIndex = 0;
}

HtmlDoc* HtmlEditor::getActiveFile() const{
    if(0<=activeIndex && activeIndex < HtmlList.size())     
        return HtmlList[activeIndex];
    else    
        return nullptr;
}

void HtmlEditor::saveSession() const{
    fs::path filePath = workplacePath;
    try{
        // check for 'saved' directory existence, and create one if it does not exist
        if(!fs::exists(filePath.parent_path())){
            fs::create_directories(filePath.parent_path());
        }
        // open config file, and clear its content (open by 'std::ios::out | std::ios::trunc')
        std::ofstream outFile(filePath);
        if(!outFile.is_open()){
            throw std::ios_base::failure("Failed to create file: " + filePath.string());
        }
        // build json file
        json settings;
        settings["activeIndex"] = activeIndex;
        settings["openFiles"] = json::array();
        for(const auto& doc: HtmlList){
            settings["openFiles"]
                .push_back({{"filePath", doc->getFilePath()}, {"showID", doc->getShowID()}});
        }

        outFile << settings.dump(4);
        outFile.close();

        std::cout << "Session saved normally.\n";
    }catch(const std::ios_base::failure& e){
        std::cerr << "Session saved with error: " << e.what() << "\n";
    }catch(const std::exception& e){
        std::cerr << "Session saved error: " << e.what() << "\n";
    }
}

void HtmlEditor::loadSession(){
    // clear current work place
    for(auto doc : HtmlList)
        delete doc;
    HtmlList.clear();
    activeIndex = -1;

    // find config file
    fs::path filePath = workplacePath;
    std::ifstream inFile(filePath);
    if(!inFile.is_open()){
        std::cerr <<"Failed to open config file" << filePath << '\n';
        std::cout<<"Open editor with blank working place\n";
        activeIndex = -1;
        return;
    }

    json settings;
    try {
        inFile >> settings;
    }catch(const json::parse_error& e){
        std::cerr << "JSON parsing error: " << e.what() << '\n';
        std::cout<<"Open editor with blank working place\n";
        return;
    }
    inFile.close();

    try{
        if(settings.contains("openFiles") && settings["openFiles"].is_array()){
            for(const auto& item : settings["openFiles"]) {
                if(item.contains("filePath") && item["filePath"].is_string() &&
                    item.contains("showID") && item["showID"].is_boolean()){
                    
                    // files are parsed lazily
                    HtmlDoc* doc = new HtmlDoc();
                    doc->setFilePath(item["filePath"]);
                    doc->setShowID(item["showID"]);
                    HtmlList.push_back(doc);
                }
            }
        }

        if(settings.contains("activeIndex") && settings["activeIndex"].is_number_integer())
            activeIndex = settings["activeIndex"];
    }catch(const json::type_error& e){
        std::cerr << "JSON format/type error: " << e.what() << '\n';
    }
    // check activeIndex to ensure it is a valid one
    if(activeIndex < 0 || activeIndex > HtmlList.size()-1){
        activeIndex = 0;
    }
    if(HtmlList.empty()){
        activeIndex = -1;
    }
    // load active file. files are loaded lazily
    if(getActiveFile())
        getActiveFile()->load(*htmlParser);

    std::cout<<"Session loaded successfully\n";
    list();
}

void HtmlEditor::saveAll(){
    for(auto doc : HtmlList){
        if(doc->hasChanges()){
            fs::path fpath = doc->getFilePath();
            std::string filename = fpath.filename().string();
            bool result = askUser(filename + " has unsaved changes", "Do you want to save them?");
            if(result){
                try{
                    doc->save();
                }catch(std::exception e){
                    std::cout<<COLOR_RED<<"saving "<<filename<<" failed\n"<<COLOR_END;
                    std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
                }
            }
        }
    }
}

void HtmlEditor::list() const{
    std::cout<<"OpenFiles:\n";
    if(HtmlList.empty()){
        std::cout<<"None\n";
        return;
    }
    for(int i=0; i<HtmlList.size(); i++){
        fs::path fpath = HtmlList[i]->getFilePath();
        std::string info = "";
        if(i == activeIndex){
            info += "  >";
        }
        else{
            info += "   ";
        }
        info += fpath.filename().string();
        if(HtmlList[i]->hasChanges())
            info += "*";
        std::cout<<info<<'\n';
    }
}

void HtmlEditor::handleCommand(const std::string& cmd){
    cmdParser.parse(cmd);
    const std::string act = cmdParser.getPositionalArgsByIdx(0);
    if(getActiveFile() == nullptr){
        if(act == "load"){
            auto args = cmdParser.getPositionalArgs();
            if(args.size() != 2){
                std::cout<<"Invalid cmd format!\n";
                return;
            }
            std::string path = cmdParser.getPositionalArgsByIdx(1);
            loadFileByPath(path);
            std::cout<<"File loaded\n";
            list();
        }else{
            std::cout<<"workplace is empty, must load a file first\n";
        }
        return;
    }

    if(act == "insert"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() < 4){
            std::cout<<"Invalid cmd format!\n";
        }
        if(!tagReg.isTagValid(args[1])){
            std::cout<<"Invalid tag!\n";
            return;
        }
        auto insertCmd = std::make_unique<InsertCommand>(args[1], args[2], args[3], cmdParser.toString(4, (int)args.size()));
        try{
            this->getActiveFile()->executeCommand(std::move(insertCmd));
            std::cout<<"Node has been inserted\n";
        }catch(std::exception e){
            std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
        }
    }
    else if(act == "append"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() < 4){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        if(!tagReg.isTagValid(args[1])){
            std::cout<<"Invalid tag!\n";
            return;
        }
        auto appendCmd = std::make_unique<AppendCommand>(args[1], args[2], args[3], cmdParser.toString(4, (int)args.size()));
        try{
            this->getActiveFile()->executeCommand(std::move(appendCmd));
            std::cout<<"Node has been appended\n";
        }catch(std::exception e){
            std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
        }
    }
    else if(act == "edit-id"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 3){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        auto editIdCmd = std::make_unique<EditIdCommand>(args[1], args[2]);
        try{
            this->getActiveFile()->executeCommand(std::move(editIdCmd));
            std::cout<<"Id has been changed\n";
        }catch(std::exception e){
            std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
        }
    }
    else if(act == "edit-text"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() < 2){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        auto editTextCmd = std::make_unique<EditTextCommand>(args[1], cmdParser.toString(2, (int)args.size()));
        try{
            this->getActiveFile()->executeCommand(std::move(editTextCmd));
            std::cout<<"Text has been changed\n";
        }catch(std::exception e){
            std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
        }
    }
    else if(act == "delete"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 2){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        auto deleteCmd = std::make_unique<DeleteCommand>(args[1]);
        try{
            this->getActiveFile()->executeCommand(std::move(deleteCmd));
            std::cout<<"Node has been deleted\n";
        }catch(std::exception e){
            std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
        }
    }
    else if(act == "spell-check"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 1){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        auto spellCheckCmd = std::make_unique<SpellCheckCommand>();
        try{
            std::cout<<"Performing spell check, any mis-spell(if found) will be listed below\n";
            this->getActiveFile()->executeCommand(std::move(spellCheckCmd));
            std::cout<<"Spell check done\n";
        }catch(std::exception e){
            std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
        }
    }
    else if(act == "print-tree"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 1){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        auto printTreeCmd = std::make_unique<PrintTreeCommand>();
        try{
            this->getActiveFile()->executeCommand(std::move(printTreeCmd));
        }catch(std::exception e){
            std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
        }
    }
    else if(act == "read"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 2){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        if(
            askUser("This command will overwrite the current content without saving your changes."
            , "Are you sure you wish to proceed?")
        ){
            try{
                this->getActiveFile()->setFilePath(cmdParser.getPositionalArgsByIdx(1));
                this->getActiveFile()->load(*htmlParser);
                std::cout<<"File read\n";
            }catch(std::exception e){
                std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
            }
        }
        return;
    }
    else if(act == "init"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 1){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        if(askUser("This command will re-init the current content without saving your changes."
            , "Are you sure you wish to proceed?")
        ){
            try{
                this->getActiveFile()->init();
                std::cout<<"File inited\n";
            }catch(std::exception e){
                std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
            }
        }
        return;
    }
    else if(act == "save"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() > 2){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        std::filesystem::path path = cmdParser.getPositionalArgsByIdx(1);
        if(path.string() == ""){
            std::cout<<"Saving " << getActiveFile()->getFilePath() <<'\n';
            saveCurrent();
            std::cout<<"File saved\n";
        }
        else if(path.parent_path().string() == ""){
            std::cout<<"Saving " + path.filename().string() + "\n";
            saveFileByPath(path.filename().string());
            std::cout<<"File saved\n";
        }
        else{
            if(askUser("The current file is: " + this->getActiveFile()->getFilePath() + "\n"
                    + "The command will save it as: " + path.string()
                    , "Are you sure you wish to proceed?")
            ){
                this->getActiveFile()->setFilePath(path.string());
                try{
                    this->getActiveFile()->save();
                    std::cout<<"File saved\n";
                }catch(std::exception e){
                    std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
                }
                return;
            }
        }
    }
    else if(act == "load"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 2){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        std::string path = cmdParser.getPositionalArgsByIdx(1);
        loadFileByPath(path);
        std::cout<<"File loaded\n";
        list();
    }
    else if(act == "close"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 1){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        if(this->getActiveFile()->hasChanges()){
            if(askUser("The current file has unsaved changes.", "Do you want to save them?")){
                saveCurrent();
            }
        }
        closeCurrent();
        std::cout<<"File closed\n";
        list();
    }
    else if(act == "edit-list"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 1){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        list();
    }
    else if(act == "edit"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 2){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        switchFileByName(cmdParser.getPositionalArgsByIdx(1));
        std::cout<<"File switched\n";
        list();
    }
    else if(act == "showid"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 2){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        auto arg = cmdParser.getPositionalArgsByIdx(1);
        if(arg == "true"){
            this->getActiveFile()->setShowID(true);
            std::cout<<"showid config switched to true\n";
        }
        else if(arg == "false"){
            this->getActiveFile()->setShowID(false);
            std::cout<<"showid config switched to false\n";
        }
        else
            std::cout<<"Invalid cmd format!\n";
    }
    else if(act == "dir-tree"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 1){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        DirTreeVisitor dirTree;
        dirTree.printTree();
    }
    else if(act == "undo"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 1){
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        try{
            this->getActiveFile()->undo();
            std::cout<<"operation undone\n";
        }catch(std::exception e){
            std::cout<<"Failed to undo\n";
            std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
        }
        return;
    }
    else if(act == "redo"){
        auto args = cmdParser.getPositionalArgs();
        if(args.size() != 1){
            std::cout<<"failed to undo\n";
            std::cout<<"Invalid cmd format!\n";
            return;
        }
        try{
            this->getActiveFile()->redo();
            std::cout<<"Operation redone\n";
        }catch(std::exception e){
            std::cout<<"failed to redo\n";
            std::cout<<COLOR_RED<<e.what()<<COLOR_END<<'\n';
        }
        return;
    }
    else{
        std::cout<<"Invalid command\n";
        // TODO: print help
    }
}

