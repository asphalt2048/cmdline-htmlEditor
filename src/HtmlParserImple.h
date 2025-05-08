#ifndef GUARD_HTMLPARSER
#define GUARD_HTMLPARSER

#include"HtmlNode.h"
#include<gumbo.h>
#include<fstream>
#include<algorithm>
#include<cctype>
#include<sstream>

class HtmlParserImple{
    private:
    public:
        HtmlParserImple() = default;
        virtual ~HtmlParserImple() = default;
        virtual HtmlNode* parse(const std::string& path) = 0;
};


class GumboParserImple : public HtmlParserImple{
    private:
        HtmlNode* convertGumboNode(GumboNode* node){
            if (node->type == GUMBO_NODE_TEXT) {
                std::string text = node->v.text.text;
                text = trim(text);
                return new TextNode(text);
            }
        
            if (node->type == GUMBO_NODE_ELEMENT) {
        
                const char* tagname = gumbo_normalized_tagname(node->v.element.tag);
                ElementNode* elem = new ElementNode(tagname ? tagname : "div");
        
                // search for id
                GumboAttribute* id_attr = gumbo_get_attribute(&node->v.element.attributes, "id");
                if (id_attr) {
                    elem->setID(id_attr->value);
                }
        
                // adding child nodes recursively
                GumboVector* children = &node->v.element.children;
                for (unsigned int i = 0; i < children->length; ++i) {
                    HtmlNode* child = convertGumboNode(static_cast<GumboNode*>(children->data[i]));
                    if (child) {
                        elem->addChild(child);
                        child->setParent(elem);
                    }
                }
        
                return elem;
            }
        
            return nullptr;
        }
        std::string trim(const std::string& s) const{
            auto start = std::find_if_not(s.begin(), s.end(), [](unsigned char ch) {
                return std::isspace(ch);
            });
            auto end = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char ch) {
                return std::isspace(ch);
            }).base();
        
            if (start >= end) {
                return "";
            }
            return std::string(start, end);
        }
    public:
        GumboParserImple() = default;
        ~GumboParserImple() override = default;

        HtmlNode* parse(const std::string& path) override{
            std::ifstream file(path);
            if (!file.is_open()) {
                return nullptr;
            }

            std::stringstream buffer;
            buffer << file.rdbuf();
            std::string htmlContent = buffer.str();
            GumboOutput* output = gumbo_parse(htmlContent.c_str());
            HtmlNode* root = convertGumboNode(output->root);
            gumbo_destroy_output(&kGumboDefaultOptions, output);
            return root;
        }
};
#endif  