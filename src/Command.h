#ifndef GUARD_COMMAND
#define GUARD_COMMAND

#include<stack>
#include<memory>
#include"HtmlDoc.h"
#include"Visitor.h"

class HtmlDoc;

class Command{
    public:
        virtual ~Command() = 0;
        virtual void execute(HtmlDoc* doc) = 0;
        virtual void unexecute(HtmlDoc* doc) = 0;
        virtual bool isUndoable();
};

class InsertCommand : public Command{
    private:
        std::string _tag;
        std::string _id;
        std::string _location;
        std::string _text;
        HtmlNode* ref;

    public:
        InsertCommand(
            const std::string& tag,
            const std::string& id,
            const std::string& location,
            const std::string& text = ""
        ):_tag(tag), _id(id), _location(location), _text(text), ref(nullptr){}

        ~InsertCommand() override= default;

        void execute(HtmlDoc* doc) override;
        void unexecute(HtmlDoc* doc) override;
};


class AppendCommand : public Command{
    private:
        std::string _tag;
        std::string _id;
        std::string _parent;
        std::string _text;
        HtmlNode* ref;

    public:
        AppendCommand(
            const std::string& tag,
            const std::string& id,
            const std::string& parent,
            const std::string& text = ""
        ):_tag(tag), _id(id), _parent(parent), _text(text), ref(nullptr){}

        ~AppendCommand() override = default;

        void execute(HtmlDoc* doc) override;
        void unexecute(HtmlDoc* doc) override;        
};

class EditIdCommand : public Command{
    private:
        std::string _oldId;
        std::string _newId;
    
    public:
        EditIdCommand(const std::string& oldId, const std::string& newId)
            :_oldId(oldId), _newId(newId){}
        
        ~EditIdCommand() override = default;

        void execute(HtmlDoc* doc) override;
        void unexecute(HtmlDoc* doc) override;
};

class EditTextCommand : public Command{
    private:
        std::string _id;
        std::string _newText;
        std::string _oldText;
        bool _oldHasError;
    public:
        EditTextCommand(const std::string& id, const std::string& newText = "")
            :_id(id), _newText(newText){}

        ~EditTextCommand() override = default;

        void execute(HtmlDoc* doc) override;
        void unexecute(HtmlDoc* doc) override;
};

class DeleteCommand : public Command{
    private:
        std::string _id;
        HtmlNode* ref;

    public:
        DeleteCommand(const std::string& id):_id(id), ref(nullptr){}

        ~DeleteCommand() override = default;

        void execute(HtmlDoc* doc) override;
        void unexecute(HtmlDoc* doc) override;    
};

class SpellCheckCommand : public Command{
    public:
        SpellCheckCommand() = default;
        ~SpellCheckCommand() override = default;

        void execute(HtmlDoc* doc) override;
        void unexecute(HtmlDoc* doc) override;

        bool isUndoable() override { return false;}
};

class PrintTreeCommand : public Command{
    public:
        PrintTreeCommand() = default;
        ~PrintTreeCommand() = default;

        void execute(HtmlDoc* doc) override;
        void unexecute(HtmlDoc* doc) override;

        bool isUndoable() override {return false;}
};
#endif