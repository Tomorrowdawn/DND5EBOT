#pragma once
#include<iostream>
#include<vector>
#include<map>
#include<combat.h>
#include <memory>
namespace DND5E{

enum Aname{
    STR=0,DEX,CON,INT,WIS,CHA
};

enum state{
    dead = 0
};

class origin_character{
    public:
        std::string name;
        int maxHP;
        std::vector<int> spell_slot;
        unsigned int ability[6];//I really want to use type char which saves memory XXDDDD
        //STR,DEX,CON,INT,WIS,CHA
        std::map<std::string,int> modifier;//record ability modifier
        unsigned int speed;
        unsigned int dark_vision;
        unsigned int blind_vision;
        bool vulnerability[10];
        bool Resistance[10];
        class DND5E::character* current;
};

class character:origin_character{
    private:
        int HP;
        int AC;
        std::vector<enum state> _state;
        std::tuple<int,int> position;
        struct origin_character* origin;//记录初始模板用于恢复
        int owner;//拥有者id
        std::vector<std::shared_ptr<Magic>> magic;//管理魔法效应
        std::vector<std::shared_ptr<Physics>> physics;//管理非魔法效应
    public:
        int get_state();
};

};