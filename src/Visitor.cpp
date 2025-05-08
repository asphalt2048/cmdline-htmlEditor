#include"Visitor.h"

HtmlVisitor::~HtmlVisitor(){}

/* =================================== Spell Checker ============================================ */

void SpellCheckVisitor::visit(ElementNode* node){
    for(const auto n : node->getChildren()){
        if(!node->isDeleted())
            n->accept(*this);
    }
}

void SpellCheckVisitor::visit(TextNode* node) {
    // I am being lazy here, I should not depends on sstream for string spliting...
    std::istringstream iss(node->getText());
    std::string word;
    bool hasError = false;
    while(iss >> word){
        for (char& c : word) {
            c = std::tolower(static_cast<unsigned char>(c));
        }
        if(!hunspell.spell(word.c_str())){
            std::cout<<COLOR_RED
                <<"Mis-spell on node["<<node->getParent()->getID()<<"]: "<<word
                <<COLOR_END<<'\n';
                hasError = true;
        }
    }
    node->setError(hasError);
}

/* =================================== Tree Print ============================================ */
void PrintTreeVisitor::visit(ElementNode* node){
    std::string indent = generateIndent(node, depth);
    std::cout<<indent<<getNodeContent(node)<<'\n';
    depth++;
    for(const auto n : node->getChildren()){
        if(!n->isDeleted())
            n->accept(*this);
    }
    depth--;
}

void PrintTreeVisitor::visit(TextNode* node){
    std::string indent = generateIndent(node, depth);
    std::cout<<indent<<getNodeContent(node)<<'\n';
}

bool PrintTreeVisitor::isLastChild(HtmlNode* node){;
    HtmlNode* parent = node->getParent();
    if(!parent) return false;
    bool foundNonDeleted = false;
    const auto siblings = parent->getChildren();
    auto it = std::find(siblings.begin(), siblings.end(), node);
    if(it == siblings.end())
    for(it++; it != siblings.end(); ++it){
        if(!((*it)->isDeleted())){
            foundNonDeleted = true;
            break;
        }
    }
    return (!foundNonDeleted);
}

std::string PrintTreeVisitor::generateIndent(HtmlNode* node, int depth){
    if(depth == -1) return "";
    if(depth == 0)  return "└── ";
    std::string head = "    ";
    std::string indent = isLastChild(node) ? "└── " : "├── ";
    HtmlNode* current = node;
    for(int i = depth - 1; i>0; i--){
        current = current->getParent();
        bool isLast = isLastChild(current);
        indent = ((current->getParent() && isLast) ? "    " : "│   ") + indent;
    }
    indent = head + indent;
    return indent;
}

std::string PrintTreeVisitor::getNodeContent(HtmlNode* node){
    std::string content = "";
    if(node->isText()){
        if(node->getError())
        content += "[" + std::string(COLOR_RED) + "x" + std::string(COLOR_END) + "]";
        content += node->getText();
        return content;
    }

    content = node->getTag();
    if(_showId)
        content += "#" + node->getID();
    return content;
}

/* =================================== Dir Tree Print ============================================ */
DirTreeVisitor::~DirTreeVisitor(){
    if(root)
    delete root;
}

HtmlNode* DirTreeVisitor::genTree(std::filesystem::path path){
    HtmlNode* root = new ElementNode(path.filename().string());
    // TODO: what?
    for(const auto& entry : std::filesystem::directory_iterator(path)){
        if(entry.is_directory()){
            root->addChild(genTree(entry.path()));
        }
        else{
            root->addChild(new TextNode(entry.path().filename().string()));
        }
    }
    return root;
}

void DirTreeVisitor::printTree(){
    std::string path = ".";
    root = genTree(path);
    root->accept(*this);
}