#ifndef GUARD_HTMLDOC
#define GUARD_HTMLDOC

#include<unordered_map>
#include<unordered_set>
#include<memory>
#include<fstream>
#include<filesystem>
#include<stack>
#include"HtmlParserImple.h"
#include"HtmlNode.h"
#include"Command.h"

/*
The HtmlDoc class holds file-independent configs like showID and filepath. 
The HtmlDoc holds those configs because they are managed separately for each html file, and many commands 
will need those to act correctly. The HtmlDoc is only responsible for offerring getter and setter. 
The Editor class will set those configs according to user's input, and save those config before exitting.

HtmlDoc also holds and manages commands history(redo stack, undo stack). 
*/
class Command;
class InsertCommand;
class AppendCommand;
class EditIdCommand;
class EditTextCommand;
class DeleteCommand;
class SpellCheckCommand;
class PrintTreeCommand;

class HtmlDoc{
    private:
        HtmlNode* root;
        std::unordered_map<std::string, HtmlNode*> idMap;
        std::string filePath;
        bool showID = true;
        size_t savedStatus = 0;
       
        // manages file independent command history
        std::stack<std::unique_ptr<Command>> undoStack;
        std::stack<std::unique_ptr<Command>> redoStack;

        bool isNodeIdsValidRecur(HtmlNode* node, std::unordered_set<std::string>& idSet) const;
        // simply add, no check
        void addNodesToMap(HtmlNode* node);
        void removeNodesFromMap(HtmlNode* node);
    public:
        HtmlDoc():root(nullptr), filePath(""), showID(true){};
        HtmlDoc(const std::string& fp, bool showId = true):filePath(fp),showID(showId){};

        ~HtmlDoc();

        // TODO: copy assignable or copy constructable?
        HtmlNode* getRoot() const;

        std::string getFilePath() const;
        void setFilePath(const std::string& fp);

        bool getShowID() const;
        void setShowID(bool s);

        
        void init(); // init the doc with a build-in format
        void save();
        void load(HtmlParserImple& parser);

        bool needParse() const;
        bool hasChanges() const;

        // return the pointer to the HtmlNode with a given id, return nullptr if no node is found
        HtmlNode* findNodeById(const std::string& id) const;

        bool isNodeIdsValid(HtmlNode* node) const;

        // insert a node next to a given node. The new node will appear before that node
        void insert(const std::string& id, HtmlNode* source);
        // append a node into another node's children. The new node will be the last child
        void append(const std::string& id, HtmlNode* source);
        void remove(const std::string& id);
        /* restore a node
        TODO: the logic here is quite weird: I can't rely on idMap in restoring, so I have to pass a
        ptr. And i have to ensure the ptr is indeed to a deleted node, not a new/never inserted one.....
        how strange  */
        void restore(HtmlNode* node);

        void changeId(const std::string& oldId, const std::string& newId);

        void executeCommand(std::unique_ptr<Command> cmd);
        void redo();
        void undo();
};

#endif