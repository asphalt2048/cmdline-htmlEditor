#include"HtmlDoc.h"
#include<queue>
#include<memory>

HtmlDoc::~HtmlDoc(){
    if(root)
        delete root;
}

HtmlNode* HtmlDoc::getRoot() const{
    return root;
}

std::string HtmlDoc::getFilePath() const{
    return filePath;
}

void HtmlDoc::setFilePath(const std::string& fp){
    filePath = fp;
}

bool HtmlDoc::getShowID() const{
    return showID;
}

void HtmlDoc::setShowID(bool s){
    showID = s;
}

void HtmlDoc::init(){
    ElementNode* html = new ElementNode("html", "html");
    ElementNode* head = new ElementNode("head", "head");
    head->addChild(new ElementNode("title", "title"));
    html->addChild(head);
    html->addChild(new ElementNode("body", "body"));
    idMap.clear();
    addNodesToMap(html);
    delete root;
    root = html;
    undoStack = {};
    redoStack = {};
    savedStatus = 0;
}

void HtmlDoc::save(){
    if(!root){
        throw std::logic_error("Can not save: root is null.");
    }
    std::filesystem::path path = filePath;
    if(!std::filesystem::exists(path.parent_path())){
        std::filesystem::create_directories(path.parent_path());
    }
    std::ofstream outFile(path);
    if (!outFile) {
        throw std::runtime_error("Failed to open file for writing: " + filePath );
    }
    outFile << root->toHtmlString();
    outFile.close();
    savedStatus = undoStack.size();
}

void HtmlDoc::load(HtmlParserImple& parser){
    std::ifstream inFile(filePath);
    if(!inFile.is_open()){
        throw std::runtime_error("Parsing error: can not open source file");
    }
    inFile.close();
    auto result = parser.parse(filePath);
    
    // save current state for rollback
    auto _idMap = idMap;
    idMap.clear();

    if(!isIdsValid(result)){
        delete result;
        idMap = _idMap;
        throw std::logic_error("Load a file with duplicate or unspecified id");
    }
    addNodesToMap(result);
    delete root;
    root = result;
    undoStack = {};
    redoStack = {};
    savedStatus = 0;
}

bool HtmlDoc::needParse() const{
    // TODO: the logic here is wierd
    if(!std::filesystem::exists(filePath))
        return false;
    if(root)
        return false;
    return true;
}

bool HtmlDoc::hasChanges() const{
    return !(undoStack.size() == savedStatus);
}

HtmlNode* HtmlDoc::findNodeById(const std::string& id) const{
    if(idMap.count(id)){
        // wow, can't use operator[] here, it's not a const op, won't compile
        return idMap.at(id);
    }
    return nullptr;
}

bool HtmlDoc::isIdsValid(HtmlNode* node) const{
    std::unordered_set<std::string> idSet;
    return isIdsValidRecur(node, idSet);    
}

// TODO: what if somebody want to insert a node before a text node?
void HtmlDoc::insert(const std::string& id, HtmlNode* source){
    HtmlNode* loc = findNodeById(id);
    if(!loc){
        throw std::runtime_error("Try to insert a node to null destination");
    }
    if(!source){
        throw std::logic_error("Try to insert a null node");
    }
    if(!isIdsValid(source)){
        throw std::logic_error("Try to insert a node with existing id");
    }
    // TODO: getParent always expects a Element node, but it returns a HtmlNode
    ElementNode* parent = dynamic_cast<ElementNode*>(loc->getParent());
    if(!parent){
        throw std::logic_error("The inserting location has no parent: " + loc->getID());
    }

    auto children = parent->getChildren();
    size_t idx = 0;
    for(const HtmlNode* node : children){
        // good job!
        if(node->isText() || !(node->getID() == id)){
            idx++;
        }else{
            break;
        }
    }
    parent->insertChild(idx, source);
    source->setParent(parent);
    addNodesToMap(source);
}

void HtmlDoc::append(const std::string& id, HtmlNode* source){
    HtmlNode* parent = findNodeById(id);
    if(!parent){
        throw std::runtime_error("Try to append a node to null destination");
    }
    if(!source){
        throw std::logic_error("Try to append a null node");
    }
    if(!isIdsValid(source)){
        throw std::logic_error("Try to append a node with existing id");
    }
    parent->addChild(source);
    addNodesToMap(source);
}

void HtmlDoc::remove(const std::string& id){
    HtmlNode* target = findNodeById(id);
    if(!target)
        throw std::logic_error("No node is found for removal");
    HtmlNode* parent = target->getParent();
    if(!parent){
        throw std::runtime_error("Failed to remove node: no parent found");
    }
    target->remove();
    removeNodesFromMap(target);
}

void HtmlDoc::restore(HtmlNode* node){
    if(!node)
        return;
    HtmlNode* parent = node->getParent();
    // A node to restore must have a valid parent still in tree
    if(!parent || !idMap.count(parent->getID()))
        return;
    if(!isIdsValid(node))
        return;
    addNodesToMap(node);
    node->restore();
}

void HtmlDoc::changeId(const std::string& oldId, const std::string& newId){
    HtmlNode* node = findNodeById(oldId);
    if(!node){
        throw std::logic_error("Edit id failed: no node with id " + oldId + " is found");
    }
    if(idMap.count(newId)){
        throw std::logic_error("Edit id failed: new id " + newId + " already exists");
    }
    idMap.erase(oldId);
    node->setID(newId);
    idMap[newId] = node;
}

void HtmlDoc::executeCommand(std::unique_ptr<Command> cmd){
    cmd->execute(this);
    if(!cmd->isUndoable())
        return;
    undoStack.push(std::move(cmd));
    redoStack = {}; 
}

void HtmlDoc::undo(){
    if(undoStack.empty())
        return;
    auto cmd = std::move(undoStack.top());
    undoStack.pop();
    cmd->unexecute(this);
    redoStack.push(std::move(cmd));
}

void HtmlDoc::redo(){
    if(redoStack.empty())
        return;
    auto cmd = std::move(redoStack.top());
    redoStack.pop();
    cmd->execute(this);
    undoStack.push(std::move(cmd));
}

void HtmlDoc::addNodesToMap(HtmlNode* node){
    if(!node || node->isText())
        return;
    for(const auto& child : node->getChildren()){
        addNodesToMap(child);
    }
    std::string id = node->getID();
    if(id.empty()){
        std::string tag = node->getTag();
        if(tag == "html"||
           tag == "head"||
           tag == "title"||
           tag == "body"){
            id = tag;
        }else
            return;
    }   
    idMap[id] = node;
}

void HtmlDoc::removeNodesFromMap(HtmlNode* node){
    if(!node)
        return;
    if(node->isText())
        return;
    for(const auto& child : node->getChildren()){
        removeNodesFromMap(child);
    }
    std::string id = node->getID();
    if(id == ""){
        std::string tag = node->getTag();
        if(tag == "html"||
           tag == "head"||
           tag == "title"||
           tag == "body"){
            id = tag;
        }else
            return;
    }   
    idMap.erase(id);
}

bool HtmlDoc::isIdsValidRecur(HtmlNode* node, std::unordered_set<std::string>& idSet) const{
    if(!node || node->isText())
        return true;
    std::string id = node->getID();
    if(id.empty()){
        std::string tag = node->getTag();
        if(tag == "html"||
        tag == "head"||
        tag == "title"||
        tag == "body"){
            id = tag;
        }else
            return false;
    }
    if(idMap.count(id) || idSet.count(id))
        return false;
    idSet.insert(id);
    for(const auto& child : node->getChildren()){
        if(!isIdsValidRecur(child, idSet))
            return false;
    }
    return true;
}