#include"TagRegistry.h"


TagRegistry::TagRegistry(){
    validTags = {
        "html", "head", "body", "div", "span", "p", "a", "img",
        "ul", "ol", "li", "table", "tr", "td", "th", "h1", "h2", "h3",
        "b", "i", "u", "em", "strong", "br", "hr", "title"
    };
}

// not needed+
TagRegistry::~TagRegistry(){}

bool TagRegistry::isTagValid(std::string tag){
    return validTags.count(tag);
}