#include<stdio.h>
#include<vector>
#pragma once
#ifndef COMBATH
#define COMBATH
#include<character.hpp>
#include <memory>

namespace DND5E{ 

enum DamageType{
    unreal = 0 , fire
};

struct base_Damage{
    std::vector<int> DamageDice;//a bunch of dice value
    enum DamageType dtype;
    int total;//确信无需对伤害骰重掷时,计算total.其他处理只涉及total.
};

class Damage{
    public:
        Damage();
        virtual void visit(DND5E::character& character);
        //对白板产生一个伤害.
        std::vector<std::shared_ptr<base_Damage>> damage_list;
    //damage is compound of muliple base_damage
    //每个damage代表同一瞬无法区分先后的伤害.用以区分次数,例如死亡豁免.
};

class Magic{
    public:
        virtual void Effect();
};
class Spell : Magic{
    public:
        virtual void Effect();//子类继承并重写此方法.
};
class Physics{};

};
#endif