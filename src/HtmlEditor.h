#ifndef GUARD_HTMLEDITOR
#define GUARD_HTMLEDITOR

#include"HtmlDoc.h"
#include"TagRegistry.h"
#include"CmdParser.h"
#include"HtmlParserImple.h"

/*
As the highest level that is interacting directly with user, this class manages 
orchestrator level responsiblities. That includes: parse commands, call command object, 
manages Html file lists, record activated file idx/ptr. But note: it does not manages IO with CMD
directly. The main function will read input and simply forward it to Editor

It also manages reading a file and saving a file. Because those actions
create/delete a HtmlDoc object, the HtmlDoc should not create/delete itself

I make this class check for tag validity before calling command object. Because I think 
tag validity check is part of input validity check, which is the responsibility of editor. 
Plus, customize tag should be effective throughout the editor
*/
class HtmlEditor{
    private:
        std::vector<HtmlDoc*> HtmlList;
        // idx to active file. The active file is always a parsed one(have a non-null root)
        int activeIndex;
        // the path to workplace file(the file recording settings/configs)
        std::string workplacePath;
        // registry for valid tags(future: can be customized to add self-defined tags)
        // future: imple tagCheckVisitor.
        // a file with invalid tags can be parsed and loaded, but those tags will be marked...
        TagRegistry tagReg;
        // parse user's input into positional args and options
        CmdParser cmdParser;
        // Bridge Mode: ptr to base class
        HtmlParserImple* htmlParser;

        bool askUser(const std::string& descrip, const std::string& ques) const;
        // return -1 if not found
        int findDocByPath(const std::string& path) const;
        int findDocByName(const std::string& filename) const;
        void build(const std::string& filename);

        
    public:
        HtmlEditor(const std::string& path = "./data/config.json")
            :workplacePath(path), htmlParser(new GumboParserImple()), activeIndex(-1){}
        ~HtmlEditor(){
            for(HtmlDoc* doc : HtmlList){
                delete doc;
            }
            delete htmlParser;
        }

        HtmlDoc* getActiveFile() const;

        
        void switchFileByName(const std::string& filename);
        void loadFileByPath(const std::string& path);
        void saveFileByPath(const std::string& filename);
        void saveCurrent();
        void saveAll();
        void closeCurrent();

        // save HtmlList, activatedFile into a config file
        void saveSession() const;
        // read config file, initialize Htmllist, activateFile, and print out read info
        void loadSession();

        // list HtmlList and activeIndex
        void list() const;

        void handleCommand(const std::string& cmd);

};

#endif