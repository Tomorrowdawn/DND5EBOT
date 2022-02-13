#ifndef _DND5E_COMMAND
#define _DND5E_COMMAND

#include <algorithm>
#include "database.hpp"
#include "dice_analyzer.hpp"

#include "magic.hpp"

class Command : public Factory< Command,std::string >{
   public:
    Command(Key){}
    static auto commands(){
        return Factory::cmds();
    }
    virtual std::string analyze() = 0;
};


std::string trim(std::string S);

namespace DND5E{
    class Parser{
       public:
        Parser() = default;
        explicit Parser(std::string S):origin(S){result = get_parse(S);};
        explicit Parser(const char* input):origin(input){};
        static std::string get_parse(std::string S);
        std::string origin;
        std::string result;
    };
    const static inline int command_limit = 30;
    const static inline int multi_dice_limit = 10;
}

#endif
