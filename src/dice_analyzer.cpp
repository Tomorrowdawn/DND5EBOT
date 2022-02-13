#include "dice_analyzer.hpp"
#include <algorithm>
#include <iterator>
#include <iostream>
#include <unordered_map>
#include <cstring>
using namespace DND5E;
using namespace std;

Dice_Expr Points_parser(Dice_Expr& points_expr,const char *relop,int num);

string brack_decorate(string& record,brack option){
    switch (option)
    {
    case brack::NO:
        return record;/* code */
    case brack::PARENS:
        record.insert(0,"(");
        return record + ")";
    case brack::BRACKET:
        record.insert(0,"[");
        return record + "]";
    default:
        return "";
    }
}



Dice_Expr Points_parser(Dice_Expr& points_expr,const char *rlop,int num){
    char relop[10];
    strcpy(relop,rlop);
    strupr(relop);
    if(strcmp(relop,"GEP")==0){
        string all_elements = points_expr.Points_print(LIST);
        string first = brack_decorate(all_elements,PARENS);
        auto ans = points_expr.GEP(num);
        string result = first + " gep " + to_string(num) + "=" + ans.Points_print(LIST);
        Dice_Expr ret(result,ans.P);
        ret.set_expr(0);
        return ret;
    }
    else if(strcmp(relop,"K")==0){
        string all_elements = points_expr.Points_print(LIST);
        string first = brack_decorate(all_elements,PARENS);
        auto ans = points_expr.K_max(num);
        string result = first + " k " + to_string(num) + "=" + ans.Points_print(ADD);
        return Dice_Expr(result,ans.P);
    }
    else if(strcmp(relop,"L")==0){
        string all_elements = points_expr.Points_print(LIST);
        string first = brack_decorate(all_elements,PARENS);
        auto ans = points_expr.L_min(num);
        string result = first + " l " + to_string(num) + "=" + ans.Points_print(ADD);
        return Dice_Expr(result,ans.P);
    }
    else{
        if(relop == NULL){throw "空的relop";}
        std::cerr << "hello" <<endl;
        std::cerr << relop <<endl;
        throw "查无此词:";
    }
}

int erracode = 0;
int recursive_height = 0;

class StaticDataBase{
   public:
    static std::unordered_map<int,int> syntax_map;
};

/*
expression规范:在所有points下均传递LIST形式;在expr下传递ADD形式.
在roll下还要做一个额外检查.因为r2d20这种形式的存在,隐形要求points转化为expr.
*/

Dice_Expr Analyzer::visit(Dnode* root){
    if(visited!=0)
        throw "no syntax tree!";
    if(strcmp(root->name,"dice")==0){
        return dice_visit(root);
    }
    else if(strcmp(root->name,"expr")==0){
        return expr_visit(root);
    }
    else if(strcmp(root->name,"expr-parens")==0){
        auto expr = expr_visit(root->children[0]);
        expr.bracket = PARENS;
        return expr;
    }
    else if(strcmp(root->name,"points-parens")==0){
        auto points = points_visit(root->children[0]);
        points.bracket = PARENS;
        return points;
    }
    else if(strcmp(root->name,"points")==0){
        return points_visit(root);
    }
    else{
        std::cerr << "Dnode name error:" << root->name << endl;
    }
    return Dice_Expr();
}

Dice_Expr Analyzer::dice_visit(Dnode* root){
    int faces = atoi(root->children[0]->name);
    if(faces>500){throw "请您造个"+to_string(faces)+"面的骰子试试捏。";}
    return Dice_Expr(faces);
}

Dice_Expr Analyzer::points_visit(Dnode* root){
    if(strcmp(root->name,"points-parens")==0){
        if(recursive_height>=15){throw "别搞破坏了.";}
        recursive_height+=1;
        auto points = points_visit(root->children[0]);
        points.bracket = PARENS;
        return points;
    }
    if(root->children_num == 1){
        //points->dice
        auto DICE = dice_visit(root->children[0]);
        auto point = DICE.roll();
        vector<int> temp = {point};
        string a = to_string(point);
        return Dice_Expr(a,temp);
    }
    else if(root->children_num == 2){
        auto DICE = dice_visit(root->children[1]);
        int num = atoi(root->children[0]->name);
        if(num <= 0 || num >= 150){throw "您搁这儿测试呢";}
        vector<int> temp;
        for(int i=0;i<num;i++){
            temp.push_back(DICE.roll());
        }
        auto T=Points(temp);
        string az = T.Points_print(LIST);
        return Dice_Expr(az,temp);
    }
    else{
        auto first = points_visit(root->children[0]);
        int num = atoi(root->children[2]->name);
        auto ans = Points_parser(first,root->children[1]->name,num);
        return ans;
    }
}

Dice_Expr Analyzer::to_dice_expr(Dnode* root){
    if(strcmp(root->name,"points")==0){
        return points_visit(root);
    }
    else{
        int num = atoi(root->name);
        vector<int> temp = {num};
        string a = to_string(num);
        return Dice_Expr(a,temp);
    }
}

Dice_Expr Analyzer::expr_visit(Dnode* root){
    if(strcmp(root->name,"expr-parens")==0){
        if(recursive_height>=15){throw "别搞破坏了.";}
        recursive_height +=1;
        auto expr = expr_visit(root->children[0]);
        expr.bracket = PARENS;
        return expr;
    }
    char* relop = root->children[1]->name;
    auto first = to_dice_expr(root->children[0]);
    auto second = to_dice_expr(root->children[2]);
    if(strcmp(relop,"+")==0)
        return first + second;
    else if(strcmp(relop,"-")==0)
        return first - second;
    else if(strcmp(relop,"*")==0)
        return first * second;
    else if(strcmp(relop,"/")==0)
        return first / second;
    else 
        throw "No such operator.";
}

/**
 * @brief 
 * 
 * @param type_control =0 means default.Analyzer will handle expression as single expression.
 * typically, 3d20 will be parsed as expr;
 * =1 means use origin form. no convertion will be done.
 */
void Analyzer::Run(int type_control){
    auto ans = visit(syntax_tree->root->children[0]);
    if(type_control==1){ans.to_expr = 0;}
    if(strcmp(syntax_tree->root->children[0]->name,"points")==0
                        && ans.count() > 1 && ans.to_expr==1){
        ans.expression = ans.Points_print(ADD);
        auto tp = ans.shrink();
        ans.P = tp.P;
    }
    result = ans;
    destroy_tree(this->syntax_tree);
    this->syntax_tree = NULL;
    visited = 1;
}

Dice_Expr Dice_Expr::operator+(const Dice_Expr& right){
    auto rec1 = this->expression;
    auto rec2 = right.expression;
    auto new_rec1 = brack_decorate(rec1,this->bracket);
    auto new_rec2 = brack_decorate(rec2,right.bracket);

    #ifdef _PAREN_DEBUG
    if(new_rec1 == ""|| new_rec2 == ""){
        std::cerr << "括号错误 paren error" << std::endl;
        std::cerr << "rec1:" << rec1 << endl;
        std::cerr << "rec2:" << rec2 << endl;
        return ;
    }
    #endif
    
    auto result = new_rec1 + "+" + new_rec2;
    auto first = this->shrink();
    auto second = right.shrink();
    auto sum = first.P[0] + second.P[0];
    vector<int> temp = {sum};
    Dice_Expr ans(result,temp);
    return ans;
}

Dice_Expr Dice_Expr::operator-(const Dice_Expr& right){
    auto rec1 = this->expression;
    auto rec2 = right.expression;
    auto new_rec1 = brack_decorate(rec1,this->bracket);
    auto new_rec2 = brack_decorate(rec2,right.bracket);
    
    auto result = new_rec1 + "-" + new_rec2;
    auto first = this->shrink();
    auto second = right.shrink();
    auto sub = first.P[0] - second.P[0];
    vector<int> temp = {sub};
    Dice_Expr ans(result,temp);
    return ans;
}


Dice_Expr Dice_Expr::operator*(const Dice_Expr& right){
    auto rec1 = this->expression;
    auto rec2 = right.expression;
    string new_rec1,new_rec2;
    if(this->count()==1){
        new_rec1 = rec1;
    }
    else{
        new_rec1 = brack_decorate(rec1,PARENS);
    }
    if(right.count()==1){
        new_rec2 = rec2;
    }
    else{
        new_rec2 = brack_decorate(rec2,PARENS);
    }
    auto result = new_rec1 + "*" + new_rec2;
    auto first = this->shrink();
    auto second = right.shrink();
    auto product = first.P[0] * second.P[0];
    vector<int> temp = {product};
    Dice_Expr ans(result,temp);
    return ans;
}


Dice_Expr Dice_Expr::operator/(const Dice_Expr& right){
    auto rec1 = this->expression;
    auto rec2 = right.expression;
    string new_rec1,new_rec2;
    if(this->count()==1){
        new_rec1 = rec1;
    }
    else{
        new_rec1 = brack_decorate(rec1,PARENS);
    }
    if(right.count()==1){
        new_rec2 = rec2;
    }
    else{
        new_rec2 = brack_decorate(rec2,PARENS);
    }
    auto result = new_rec1 + "/" + new_rec2;
    auto first = this->shrink();
    auto second = right.shrink();
    auto div = first.P[0] / second.P[0];
    vector<int> temp = {div};
    Dice_Expr ans(result,temp);
    return ans;
}




Points Points::shrink() const{
    int sum = 0;
    for(int i=0;i<P.size();i++){
        sum+=P[i];
    }
    vector<int> temp = {sum};
    return Points(temp);
}

Points Points::GEP(int boundary) const{
    vector<int> temp;
    std::cerr << this->Points_print(LIST) <<endl;
    copy_if(P.begin(),P.end(),back_inserter(temp),
                [boundary](int element){return element >= boundary;});
    if(temp.empty())
        temp.push_back(0);
    return Points(temp);
}

Points Points::K_max(int K) const{
    if(K<=0){throw "K_MAX negative";}
    if(K>P.size()){vector<int> temp(P);return Points(temp);}
    vector<int> temp(P);
    nth_element(temp.begin(),temp.begin()+K-1,temp.end(),
                            [](int A,int B){return A > B;});
    std::vector<int>::iterator i = temp.begin();
    std::vector<int>::iterator j = temp.begin()+K;
    return Points(i,j);
}

Points Points::L_min(int L) const{
    if(L<=0) {throw "L_MIN negative";}
    if(L>P.size()){vector<int> temp(P);return Points(temp);}
    vector<int> temp(P);
    nth_element(temp.begin(),temp.begin()+L-1,temp.end());
    std::vector<int>::iterator i = temp.begin();
    std::vector<int>::iterator j = temp.begin()+L;
    return Points(i,j);
}

string Points::Points_print(Print_control config) const{
    string temp;
    switch(config){
        case Print_control::ADD :{
            temp = to_string(P[0]);
            for(int i=1;i<P.size();i++){
                temp += "+" + to_string(P[i]);
            }
        }break;
        case Print_control::LIST :{
            temp = to_string(P[0]);
            for(int i=1;i<P.size();i++){
                temp+= "," + to_string(P[i]);
            }
        }break;
        default:break;
    }
    return temp;
}
