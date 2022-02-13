#ifndef DICEPARSE
#define DICEPARSE
#define MAX_COMMAND_LENGTH 30

#include<stdio.h>

struct _dice_command_node{
    char name[MAX_COMMAND_LENGTH];
    struct  _dice_command_node *parent;
    struct  _dice_command_node *children[10];
    int children_num;
};

typedef struct _dice_command_node Dnode;

struct _Dice_command{
    struct _dice_command_node* root;
};
typedef struct _Dice_command Dice_command;

Dnode *Dnode_new(const char *text);
int Dnode_add_child(Dnode* parent,Dnode* child);
void print_Dnode(FILE * fout, Dnode * node, int level);
void print_command(FILE * fout, Dice_command * tree);
void destroy_tree(Dice_command* tree);

#endif