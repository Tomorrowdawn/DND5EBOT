#include "command.hpp"
#include <cstring>

using namespace DND5E;
using namespace std;

string trim(string S){
    int i,j;
    for(i=0;i<S.length();){
        if(isspace(S[i]))
            i++;
        else
            break;
    }
    for(j=S.length()-1;j>=i;){
        if(isspace(S[j]))
            j--;
        else
            break;
    }
    return S.substr(i,j-i+1);
}

string Parser::get_parse(std::string S){
    //需要重写
    //实现一个最长前缀匹配函数用来寻找合法的解析子类,
    //然后调用auto X = fetch(name,S);return X->analyze();
    string ans = trim(S);
    S = ans+ "\n";
    if(S.length()>command_limit)
        throw "命令过长!";
    if(S.rfind("r",0)==0){
        S.erase(S.begin(),S.begin()+1);
        auto hash = S.find("#");
        if(hash != string::npos){
            string first = S.substr(0,hash);
            string second = S.substr(hash+1);
            int times;
            try{
                times = stoi(first);
            }catch(...){
                throw "#前数字表达式错误!";
            }
            if(times > multi_dice_limit)
                throw "骰得太多,bot累了";
            vector<Dice_Expr> multi_dice;
            for(int i=0;i<times;i++){
                auto syntax_tree = parse(second.data());
                if(!syntax_tree)
                    throw "掷骰表达式有误!";
                Analyzer handler(syntax_tree);
                handler.Run();
                multi_dice.push_back(handler.result);
            }
            ans += "=\n";
            for(auto res:multi_dice){
                ans+= res.expression;
                if(res.to_expr==1){
                    ans+= "=" + to_string(res.P[0]);
                }
                ans += "\n";
            }
            return ans;
        }
        else{
            auto syntax_tree = parse(S.data());
            if(!syntax_tree)
                throw "掷骰表达式有误!";
            Analyzer handler(syntax_tree);
            handler.Run();
            if(handler.result.to_expr==1){
                ans+="="+handler.result.expression+"="+to_string(handler.result.P[0])+"\n";
            }
            else
                ans+="="+handler.result.expression+"\n";
        }
        return ans;
    }
    return ans;
}