
#ifdef TEST
extern "C"{
    #include "dice.h"
}
#include "dice_analyzer.hpp"
#include <string>
#include <iostream>
#include <cstring>

using namespace DND5E;

int main(void){
    while(1){
        char in[1000];
        std::cin.getline(in,500);
        if(strcmp(in,"qt")==0) break;
        int len = strlen(in);
        in[len] = '\n';
        in[len+1] = '\0';
        auto gt = parse(in);
        Analyzer pin(gt);
        try{
            pin.Run();
            std::cout << pin.record << std::endl;
            std::cout << pin.result.P[0] << std::endl;
            print_command(stdout,gt);
        }catch(const char* msg){
            std::cerr << msg << std::endl;
        }
    }
}

#endif