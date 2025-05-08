#ifndef GUARD_TAGREGISTRY
#define GUARD_TAGREGISTRY

#include<unordered_set>
#include<string>

// This class holds all valid html tags, with isTagValid to validate user's input
class TagRegistry
{
private:
    std::unordered_set<std::string> validTags;
public:
    TagRegistry();
    ~TagRegistry();
    bool isTagValid(std::string tag);
};

#endif