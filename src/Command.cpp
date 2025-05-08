#include"Command.h"

// TODO: how much show HtmlDoc manages??? 

Command::~Command(){}

bool Command::isUndoable(){
    return true;
}

/* =============================== Insert command ============================ */
void InsertCommand::execute(HtmlDoc* doc){
    if(ref){
        doc->restore(ref);
        return;
    }
    ElementNode* source = new ElementNode(_tag, _id);
    if(_text != ""){
        source->insertChild(0, new TextNode(_text));
    }
    try{
        doc->insert(_location, source);
    }catch(std::exception e){
        delete source;
        throw e;
        return;
    }
    ref = source;
}

void InsertCommand::unexecute(HtmlDoc* doc){
    doc->remove(_id);
}

/* =============================== Append command ============================ */
void AppendCommand::execute(HtmlDoc* doc){
    if(ref){
        doc->restore(ref);
        return;
    }
    ElementNode* source = new ElementNode(_tag, _id);
    if(_text != ""){
        source->insertChild(0, new TextNode(_text));
    }
    try{
        doc->append(_parent, source);
    }catch(std::exception e){
        delete source;
        throw e;
        return;
    }
    ref = source;
}

void AppendCommand::unexecute(HtmlDoc* doc){
    doc->remove(_id);
}

/* =============================== Edit-id command ============================ */
void EditIdCommand::execute(HtmlDoc* doc){
    doc->changeId(_oldId, _newId);
}

void EditIdCommand::unexecute(HtmlDoc* doc){
    doc->changeId(_newId, _oldId);
}


/* =============================== Edit-text command ============================ */
void EditTextCommand::execute(HtmlDoc* doc){
    HtmlNode* node = doc->findNodeById(_id);
    if(!node){
        throw std::logic_error("No node with id: " + _id + " is found");
    }
    this->_oldText = node->getText();
    node->setText(_newText);
    _oldHasError = node->getError();
    node->setError(false);
}

void EditTextCommand::unexecute(HtmlDoc* doc){
    HtmlNode* node = doc->findNodeById(_id);
    if(!node){
        throw std::runtime_error("Failed to find node in EditIdCommand::unexecute");
    }
    node->setText(_oldText);
    node->setError(_oldHasError);
}

/* =============================== delete command ============================ */
void DeleteCommand::execute(HtmlDoc* doc){
    HtmlNode* node = doc->findNodeById(this->_id);
    if(!node){
        throw std::logic_error("Try to remove a non-existing node");
    }
    doc->remove(_id);
    ref = node;
}

void DeleteCommand::unexecute(HtmlDoc* doc){
    doc->restore(ref);
}

/* =============================== spell-checker command ============================ */
void SpellCheckCommand::execute(HtmlDoc* doc){
    SpellCheckVisitor sc;
    doc->getRoot()->accept(sc);
}

void SpellCheckCommand::unexecute(HtmlDoc* doc){return;}

/* =============================== Print-tree command ============================ */
void PrintTreeCommand::execute(HtmlDoc* doc){
    PrintTreeVisitor pt(doc->getShowID());
    doc->getRoot()->accept(pt);
}

void PrintTreeCommand::unexecute(HtmlDoc* doc){return;}