#ifndef AVL_H
#define AVL_H

#include "types.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct AVLNode {
    int start;
    int end;
    struct AVLNode *left;
    struct AVLNode *right;
    int height;
} AVLNode;

AVLNode* avl_insert(AVLNode* node, int start, int end, bool* success);
bool avl_check_collision(AVLNode* root, int start, int end);
int avl_suggest_time(AVLNode* root, int duration, int day_start_constraint);
void avl_free(AVLNode* node);

#endif
