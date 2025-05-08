#include"HtmlNode.h"

/* ====================================== HtmlNode base =================================== */
HtmlNode::~HtmlNode(){}

bool HtmlNode::isDeleted() const{
    return deleted;
}

HtmlNode* HtmlNode::getParent() const{
    return parent;
}

void HtmlNode::setError(bool val){
    hasError = val;
}

bool HtmlNode::getError() const{
    return hasError;
}

void HtmlNode::setParent(HtmlNode* parent){
    this->parent = parent;
}

void HtmlNode::addChild(HtmlNode* node){
    throw std::logic_error("This node has no child\n");
}

void HtmlNode::removeChild(size_t idx){
    throw std::logic_error("This node has no child\n");
}

std::vector<HtmlNode*>& HtmlNode::getChildren(){
    throw std::logic_error("This node has no child\n");
}

std::string HtmlNode::getID() const {
    throw std::logic_error("Text node have no id");
}

void HtmlNode::setID(const std::string& newID){
    throw std::logic_error("Can not set, this node have no id");
}

std::string HtmlNode::getTag() const{
    throw std::logic_error("This node have no tag");
}

/* ========================================= ElementNode =========================================== */
void ElementNode::remove(){
    for(auto& n : children){
        if(n) n->remove();
    }
    deleted = true;
}

void ElementNode::restore(){
    for(auto& n : children){
        if(n) n->restore();
    }
    deleted = false;
}

bool ElementNode::isElement() const {
    return true;
}

bool ElementNode::isText() const {
    return false;
}

std::string ElementNode::getText() const {
    if(children.empty()){
        return "";
    }
    // TODO: may change the logic here
    // currently this method will return all text nested inside
    return children.front()->getText();
}

void ElementNode::setText(const std::string& txt){
    if(children.empty()){
        if(txt == "")
            return;
        this->addChild(new TextNode(txt));
    }
    else{
        if(children.front()->isText()){
            children.front()->setText(txt);
            if(txt == "")
                children.front()->remove();
            else
                children.front()->restore();
        }
        else{
            if(txt == "")
                return;
            children.insert(children.begin(), new TextNode(txt));
        }
    }
}

void ElementNode::addChild(HtmlNode* node){
    children.push_back(node);
    node->setParent(this);
}

void ElementNode::removeChild(size_t idx){
    if(idx < children.size()){
        children[idx]->remove();
    }
}

std::vector<HtmlNode*>& ElementNode::getChildren(){
    return children;
}

std::string ElementNode::getID() const{
    if(id.empty())
        return tag;
    return id;
}

void ElementNode::setID(const std::string& newID){
    if(newID != "")
        this->id = newID;
}

std::string ElementNode::getTag() const{
    return tag;
}

void ElementNode::insertChild(size_t idx, HtmlNode* node){
    if(idx <= children.size()){
        children.insert(children.begin() + idx, node);
        node->setParent(this);
    }else{
        throw std::out_of_range("idx out of range");
    }
}

void ElementNode::accept(HtmlVisitor& visitor){
    visitor.visit(this);
}

std::string ElementNode::toHtmlString(int indent){
    if(deleted) return "";
    std::string result(indent, ' ');
    result += "<" + tag;
    if(id != "")
        result += " id=\"" + id + "\"";
    result += ">\n";
    for(const auto n : children){
        result += n->toHtmlString(indent + 2);
    }
    result += std::string(indent, ' ') + "</" + tag + ">\n";
    return result;
}

/* =================================== TextNode ======================================= */
void TextNode::remove(){
    deleted = true;
}

void TextNode::restore(){
    deleted = false;
}

bool TextNode::isElement() const {
    return false;
}

bool TextNode::isText() const {
    return true;
}

std::string TextNode::getText() const{
    return text;
}

void TextNode::setText(const std::string& txt){
    text = txt;
}

void TextNode::accept(HtmlVisitor& visitor){
    visitor.visit(this);
}

std::string TextNode::toHtmlString(int indent){
    if(deleted) return "";
    return std::string(indent, ' ') + text + "\n";
}