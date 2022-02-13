#ifndef _DICE_ANALYZER
#define _DICE_ANALYZER

extern "C"{
    #include "dice.h"
    extern Dice_command *parse(const char *input_string);
    //解析失败返回NULL
}

#include <vector>
#include <memory>
#include <string>

namespace DND5E{
    enum Print_control{ADD = 1,LIST};
    enum brack {NO=0,PARENS,BRACKET};
class Dice{
   public:
    Dice():faces(6){}
    explicit Dice(int num):faces(num){}
    int faces;
    inline int roll(){return rand()%faces + 1;}
};
class Points:public Dice{
   public:
    Points():Dice(){};
    Points(int* a,int num):P(a,a+num*sizeof(int)){}
    explicit Points(std::vector<int>& P1):P(P1){}
    Points(std::vector<int>::iterator& begin,std::vector<int>::iterator& end):
            P(begin,end){}
    Points(int num):Dice(num){}
    inline bool isempty(){return !P.empty();}
    std::vector<int> P;
    std::string Points_print(Print_control config) const;
    Points shrink() const;
    Points GEP(int boundary) const;
    Points K_max(int K) const;
    Points L_min(int L) const;
    inline int count() const{return P.size();}
};

class Dice_Expr:public Points{
   public:
    Dice_Expr():Points(),to_expr(1),bracket(NO){}
    Dice_Expr(int num):Points(num),to_expr(1),bracket(NO){}
    Dice_Expr(std::string& expr,std::vector<int>& P1):
                        expression(expr),Points(P1),to_expr(1),bracket(NO){};
    std::string expression;
    int to_expr;//=0表示不用转为expr,=1表示需要转为expr.默认情况下to_expr=1,如果不转需要手动改.
    brack bracket;//记录外层括号。0表示不用括号，1圆括号，2方括号...
    inline std::string expr_print(){return expression;}
    inline void set_expr(int num){to_expr = num;}
    Dice_Expr operator+(const Dice_Expr& right);
    Dice_Expr operator-(const Dice_Expr& right);
    Dice_Expr operator*(const Dice_Expr& right);
    Dice_Expr operator/(const Dice_Expr& right);
    //因为骰子计算是需要记录表达式的,所以只能返回Dice_Expr.
};

class Analyzer{
   public:
    Analyzer() = default;
    explicit Analyzer(Dice_command* tree):syntax_tree(tree){};
    //std::string record;
    Dice_Expr result;
    Dice_command* syntax_tree;
    void Run(int type_control = 0);
    Dice_Expr to_dice_expr(Dnode* root);
    Dice_Expr visit(Dnode* root);
    Dice_Expr dice_visit(Dnode* root);
    Dice_Expr points_visit(Dnode* root);
    Dice_Expr expr_visit(Dnode* root);
    int visited = 0;
};

//Dice_Expr Points_parser(Dice_Expr& points_expr,const char *relop,int num);

#ifdef _NUMBER_SPEC

Dice_Expr operator+(const Dice_Expr& left,const int right);
Dice_Expr operator-(const Dice_Expr& left,const int right);
Dice_Expr operator*(const Dice_Expr& left,const int right);
Dice_Expr operator/(const Dice_Expr& left,const int right);

Dice_Expr operator+(const int left,const Dice_Expr& right);
Dice_Expr operator-(const int left,const Dice_Expr& right);
Dice_Expr operator*(const int left,const Dice_Expr& right);
Dice_Expr operator/(const int left,const Dice_Expr& right);
#endif

}

#endif