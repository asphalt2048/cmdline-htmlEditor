#ifndef GUARD_MYPARSER
#define GUARD_MYPARSER

#include<vector>
#include<unordered_map>
#include<string>
#include<sstream>

/*
This class parse input(user's command). What it does is simply spliting a string into positional args 
and options. 
For example, 'print -showid false' will become:
    positionalArgs: {"print"}
    options;{["showid": "false"]}

Note, this class's responsibility is simply parse a cmd, it will not call on any object to carry out 
the cmd. 
*/

class CmdParser
{
private:
    std::vector<std::string> positionalArgs;
    std::unordered_map<std::string, std::string> options;
public:
    CmdParser() = default;
    ~CmdParser() = default;

    void parse(const std::string& cmd){
        // clear the result of last time
        options.clear();
        positionalArgs.clear();

        std::istringstream iss(cmd);
        std::string token;
        std::string currentOpt;

        while(iss >> token){
            if(!token.empty() && token[0] == '-'){
                // record as "-option xxx"
                currentOpt = token;
                options[currentOpt] = "";
            }else{
                if(!currentOpt.empty()){
                    options[currentOpt] = token;
                    currentOpt.clear();
                }else{
                    positionalArgs.push_back(token);
                }
            }
        }        
    }

    std::vector<std::string> getPositionalArgs() const{
        return positionalArgs;
    }
    // use "-option" to access corresponding option
    std::string getOption(const std::string& option) const{
        if(options.find(option) != options.end()){
            return options.at(option);
        }
        return "";
    }
    std::string getPositionalArgsByIdx(unsigned idx) const{
        if(idx<positionalArgs.size()){
            return positionalArgs[idx];
        }
        return "";
    }

    std::string toString(int begin, int end){
        if(begin >= positionalArgs.size())
            return "";
        if(end <= 0)
            return "";
        if(end > (int)positionalArgs.size())
            end = (int)positionalArgs.size();
        if(begin < 0)
            begin = 0;
        std::ostringstream oss;
        for(int i = begin; i<end; i++){
            oss << positionalArgs[i];
            if(i != end-1)
                oss << ' ';
        }

        return oss.str();
    }
};

#endif
