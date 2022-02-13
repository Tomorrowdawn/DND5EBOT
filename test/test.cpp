#define TEST
#ifdef TEST

#include <string>
#include <iostream>
#include <cstring>
#include "command.hpp"

using namespace DND5E;

int main(void){
    while(1){
        char in[1000];
        std::cin.getline(in,500);
        if(strcmp(in,"qt")==0) break;
        int len = strlen(in);
        in[len] = '\n';
        in[len+1] = '\0';
        char* input = strupr(in);
        std::string ans;
        try{ ans = DND5E::Parser::get_parse(input);}catch(const char* msg){
            std::cerr << msg << std::endl;
        }
        std::cout << ans <<std::endl;
    }
}

#endif