#ifndef GUARD_SPELLCHECKER
#define GUARD_SPELLCHECKER

#include"HtmlNode.h"
#include<hunspell/hunspell.hxx>
#include<unordered_set>
#include<sstream>
#include<filesystem>
#include<iostream>

class HtmlNode;
class ElementNode;
class TextNode;

#define COLOR_RED "\033[21;31m"
#define COLOR_END "\033[0m"

class HtmlVisitor{
    public:
        virtual ~HtmlVisitor() = 0;
        virtual void visit(ElementNode* node) = 0;
        virtual void visit(TextNode* node) = 0;
};


class SpellCheckVisitor : public HtmlVisitor{
    private:
        Hunspell hunspell;
        
    public:
        SpellCheckVisitor()
                : hunspell("./data/en_US.aff", "./data/en_US.dic"){
                }
        ~SpellCheckVisitor() override = default;

        // currently, I will simply print node's id and mis-spell word in checking functions
                
        void visit(ElementNode* node) override;
        void visit(TextNode* node) override;
};


class PrintTreeVisitor : public HtmlVisitor{
    private:
        int depth = -1;
        int _showId;
        std::string generateIndent(HtmlNode* node, int depth);
        static bool isLastChild(HtmlNode* node);
        std::string getNodeContent(HtmlNode* node);
    public:
        PrintTreeVisitor(bool showId = false):depth(-1), _showId(showId){}
        ~PrintTreeVisitor() = default;

        void visit(ElementNode* node) override;

        void visit(TextNode* node) override;
};


class DirTreeVisitor : public PrintTreeVisitor{
    private:
        HtmlNode* root;
        HtmlNode* genTree(std::filesystem::path path);
    public:
        DirTreeVisitor() = default;
        ~DirTreeVisitor();

        void printTree();

};

#endif