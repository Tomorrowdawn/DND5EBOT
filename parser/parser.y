%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "dice.h"
// external functions from lex
extern int yylex();
extern int yyrestart();
extern int yyparse();
typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(char * str);
extern void yy_delete_buffer(YY_BUFFER_STATE buffer);
extern FILE * yyin;
extern char * yytext;
Dice_command *tree;

Dnode* one;//temp one

// Error reporting
void yyerror(const char *s);
Dnode* node(const char *node_name, int children_num, ...);
%}

%union{
    struct _dice_command_node *node;
}


%type <node> dice roll points
%type <node> expr 
%token <node> number DICE ADD MUL DIV SUB 
%token <node> LPAREN RPAREN GEP K L END
%start roll

%right GEP K L
%left ADD SUB
%left MUL DIV
%right DICE

%%

roll:expr END {tree->root = node("roll",1,$1);} 
|points END {tree->root = node("roll",1,$1);}
|END;

dice:DICE number {$$ = node("dice",1,$2);}
;

points:number dice {$$ = node("points",2,$1,$2);}
| dice {$$ = node("points",1,$1);}
| points GEP number {$$ = node("points",3,$1,$2,$3);}
| points K number {$$ = node("points",3,$1,$2,$3);}
| points L number {$$ = node("points",3,$1,$2,$3);}
| LPAREN points RPAREN {$$ = node("points-parens",1,$2);}
;

expr: points ADD points{$$ = node("expr",3,$1,$2,$3);}
|points ADD number{$$ = node("expr",3,$1,$2,$3);}
|number ADD points{$$ = node("expr",3,$1,$2,$3);}
|points SUB points{$$ = node("expr",3,$1,$2,$3);}
|points SUB number{$$ = node("expr",3,$1,$2,$3);}
|number SUB points{$$ = node("expr",3,$1,$2,$3);}
|points MUL points{$$ = node("expr",3,$1,$2,$3);}
|points MUL number{$$ = node("expr",3,$1,$2,$3);}
|number MUL points{$$ = node("expr",3,$1,$2,$3);}
|points DIV points{$$ = node("expr",3,$1,$2,$3);}
|points DIV number{$$ = node("expr",3,$1,$2,$3);}
|number DIV points{$$ = node("expr",3,$1,$2,$3);}
|number ADD number{$$ = node("expr",3,$1,$2,$3);}
|number SUB number{$$ = node("expr",3,$1,$2,$3);}
|number MUL number{$$ = node("expr",3,$1,$2,$3);}
|number DIV number{$$ = node("expr",3,$1,$2,$3);}
|LPAREN expr RPAREN{$$ = node("expr-parens",1,$2);}
;

%%

void yyerror(const char *s){
    fprintf(stderr,"表达式有误:%s\n",s);
}

/// Parse input from file `input_path`, and prints the parsing results
/// to stdout.  If input_path is NULL, read from stdin.
///
/// This function initializes essential states before running yyparse().

#ifdef _FLEX_FILE

Dice_command *parse(const char *input_path)
{
    if (input_path != NULL) {
        if (!(yyin = fopen(input_path, "r"))) {
            fprintf(stderr, "[ERR] Open input file %s failed.\n", input_path);
            exit(1);
        }
    } else {
        yyin = stdin;
    }
    tree = (Dice_command*)malloc(sizeof(Dice_command));
    yyrestart(yyin);
    if(yyparse()==1){
        free(tree);
        return NULL;
    }
    return tree;
}

#else
Dice_command *parse(const char *input_string)
{
    YY_BUFFER_STATE buffer = yy_scan_string(input_string);
    
    tree = (Dice_command*)malloc(sizeof(Dice_command));
    if(yyparse()==1){
        free(tree);
        yy_delete_buffer(buffer);
        return NULL;
    }
    yy_delete_buffer(buffer);
    return tree;
}
#endif

Dnode *node(const char *name, int children_num, ...)
{
    Dnode *p = Dnode_new(name);
    Dnode *child;
    if (children_num == 0) {
        child = Dnode_new("epsilon");
        Dnode_add_child(p, child);
    } else {
        va_list ap;
        va_start(ap, children_num);
        for (int i = 0; i < children_num; ++i) {
            child = va_arg(ap, Dnode *);
            Dnode_add_child(p, child);
        }
        va_end(ap);
    }
    return p;
}
