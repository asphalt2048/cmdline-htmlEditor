#ifndef GUARD_HTMLNODE
#define GUARD_HTMLNODE

#include<vector>
#include<string>
#include<stdexcept>
#include"Visitor.h"

class HtmlVisitor;
class SpellCheckVisitor;
class PrintTreeVisitor;

// Base class for composition mode
class HtmlNode{
    private:
        HtmlNode* parent;
        // TODO: the node itself should not hold states. Error records should be manages by HtmlDoc
        bool hasError;
        
    protected:
        bool deleted;
        // We should not define childern here, since that cause space penalty for leaves

    public:
        HtmlNode(HtmlNode* parent = nullptr):parent(parent), deleted(false), hasError(false){};
        virtual ~HtmlNode() = 0;

        virtual void remove() = 0;
        virtual void restore() = 0;
        bool isDeleted() const;

        HtmlNode* getParent() const;
        void setParent(HtmlNode* parent); 

        void setError(bool val);
        bool getError() const;

        // methods that return type
        virtual bool isElement() const = 0;
        virtual bool isText() const = 0;

        virtual std::string getText() const = 0;
        virtual void setText(const std::string& txt) = 0;

        // Throw an exception by default, as what is expected for leaf node. Non-leaf node should override them
        // methods that manages childern
        virtual void addChild(HtmlNode* node);
        virtual void removeChild(size_t idx);
        virtual std::vector<HtmlNode*>& getChildren();

        virtual std::string getID() const;
        virtual void setID(const std::string& newID);
        virtual std::string getTag() const;

        virtual void accept(HtmlVisitor& checker) = 0;
        // return the string representation
        virtual std::string toHtmlString(int indent = 0) = 0;
        //and so on...

};


class ElementNode : public HtmlNode{
    private:
        std::string tag;
        std::string id;
        // TODO: replace with unique_ptr. But use this version for now
        std::vector<HtmlNode*> children;

    public:
        // I have decided not to provide a default constructor, 
        // all element node must be initialized with tag

        ElementNode(std::string tag, std::string id=""):tag(tag), id(id){}
        ~ElementNode() override {
            for(auto node: children)
                delete node;
        };
        
        void remove() override;
        void restore() override;

        bool isElement() const override;
        bool isText() const override;

        std::string getText() const override;
        void setText(const std::string& txt) override;

        void addChild(HtmlNode* node) override;
        void removeChild(size_t idx) override;
        std::vector<HtmlNode*>& getChildren() override;

        std::string getID() const override;
        void setID(const std::string& newID) override;
        std::string getTag() const override;
        
        // TODO: should this be a part of HtmlNode? should it exist?
        void insertChild(size_t idx, HtmlNode* node);

        void accept(HtmlVisitor& visitor) override;

        std::string toHtmlString(int indent = 0) override;
};


class TextNode : public HtmlNode{
    private:
        std::string text;
        // TextNode has no tag or child
        // for current implementation, it has no id either

    public:
        TextNode(const std::string& txt = ""):text(txt){}
        ~TextNode() override = default;

        void remove() override;
        void restore() override;

        bool isElement() const override;
        bool isText() const override;

        std::string getText() const override;
        void setText(const std::string& txt) override;

        void accept(HtmlVisitor& visitor) override;

        std::string toHtmlString(int indent = 0) override;
};

#endif