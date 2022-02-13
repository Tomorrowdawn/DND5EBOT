#include "command.hpp"
#include <cstring>

using namespace DND5E;
using namespace std;


std::string DND5E::name_match(const std::string& input,std::vector<std::string>& cmds){
    std::string head;
    int needed[200] = {0};
    for(int i=0;i<= input.size();i++){
        // must match '\0'
        for(int j=0;j<cmds.size();j++){
            if(needed[j]!=0)
                continue;
            if(cmds[j][i]!=input[i]){
                needed[j] = 1;
            }
        }
        int count = 0;
        int cmd = 0;
        for(int j=0;j<cmds.size();j++){
            if(needed[j]==0){
                count++;
                cmd = j;
            }
        }
        if(count == 1)
            return cmds[cmd];
        else if(count == 0)
            throw "no command match";
    }
    throw "no command match";
}

string trim(string S){
    int i,j;
    for(i=0;i<S.length();){
        if(isspace(static_cast<unsigned char>(S[i])))
            i++;
        else
            break;
    }
    for(j=S.length()-1;j>=i;){
        if(isspace(static_cast<unsigned char>(S[j])))
            j--;
        else
            break;
    }
    return S.substr(i,j-i+1);
}

class roll_core:public Command::Registrar<roll_core>{
   public:
    roll_core(string _cmd):S{_cmd}{}
    string S;
    static string name;
    virtual string analyze() final;
};
string roll_core::name = "R";

string roll_core::analyze(){
    string ans{S};
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

string Parser::get_parse(std::string S){
    //这里传入的S已被大写处理.
    //需要重写
    //实现一个最长前缀匹配函数用来寻找合法的解析子类,
    //然后调用auto X = fetch(name,S);return X->analyze();
    string ans = trim(S);
    S = ans+ "\n";
    if(S.length()>command_limit){
        throw "命令过长!";
    }
    auto X = Command::fetch(S,S);
    return X->analyze();
}