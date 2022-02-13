#include<string.h>
#include<stdlib.h>
#include "dice.h"

Dnode *Dnode_new(const char* text){
    Dnode * new_node = (Dnode *)malloc(sizeof(Dnode));
	if (text)
		strncpy(new_node->name, text, MAX_COMMAND_LENGTH);
	else
		new_node->name[0] = '\0';
	new_node->children_num = 0;
	return new_node;
}

int Dnode_add_child(Dnode*parent,Dnode* child){
    if (!parent || !child)	return -1;
	parent->children[parent->children_num++] = child;
	return parent->children_num;
}

void print_Dnode(FILE * fout, Dnode * node, int level)
{
	// assume fout valid now
	
	// check if "node" empty pointer
	if (!node)	return;
	// print myself
	int i;
	for (i = 0; i < level; i++) {
		fprintf(fout, "|  ");
	}
	fprintf(fout, ">--%s %s %d\n", (node->children_num ? "+" : "*"), node->name,node->children_num);
	for (i = 0; i < node->children_num; i++) {
		print_Dnode(fout, node->children[i], level + 1);
	}
}

void print_command(FILE * fout, Dice_command * tree)
{
	if (!fout)	return;
	print_Dnode(fout, tree->root, 0);
}

void free_Dnode(Dnode* root){
    int i;
    if(!root)
        return;
    for(i=0;i< root->children_num;i++){
        free_Dnode(root->children[i]);
    }
    free(root);
}

void destroy_tree(Dice_command* tree){
    if(!tree)
        return;
    if(tree->root)
        free_Dnode(tree->root);
    free(tree);
}
