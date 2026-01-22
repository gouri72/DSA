#include "avl.h"

// Static helpers
static int max_val(int a, int b) { return (a > b) ? a : b; }
static int height(AVLNode *N) { return (N == NULL) ? 0 : N->height; }
static int get_balance(AVLNode* N) { return (N == NULL) ? 0 : height(N->left) - height(N->right); }

static AVLNode* new_node(int start, int end) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    node->start = start;
    node->end = end;
    node->left = NULL;
    node->right = NULL;
    node->height = 1;
    return node;
}

static AVLNode* right_rotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    y->height = max_val(height(y->left), height(y->right)) + 1;
    x->height = max_val(height(x->left), height(x->right)) + 1;
    return x;
}

static AVLNode* left_rotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    x->height = max_val(height(x->left), height(x->right)) + 1;
    y->height = max_val(height(y->left), height(y->right)) + 1;
    return y;
}

AVLNode* avl_insert(AVLNode* node, int start, int end, bool* success) {
    if (node == NULL) return new_node(start, end);

    if (start < node->start)
        node->left = avl_insert(node->left, start, end, success);
    else if (start > node->start)
        node->right = avl_insert(node->right, start, end, success);
    else {
        // No duplicate start times allowed in our simplified logic
        *success = false;
        return node;
    }

    node->height = 1 + max_val(height(node->left), height(node->right));
    int balance = get_balance(node);

    if (balance > 1 && start < node->left->start) return right_rotate(node);
    if (balance < -1 && start > node->right->start) return left_rotate(node);
    if (balance > 1 && start > node->left->start) {
        node->left = left_rotate(node->left);
        return right_rotate(node);
    }
    if (balance < -1 && start < node->right->start) {
        node->right = right_rotate(node->right);
        return left_rotate(node);
    }
    return node;
}

bool avl_check_collision(AVLNode* root, int start, int end) {
    if (root == NULL) return false;
    
    // Overlap: (StartA < EndB) and (EndA > StartB)
    if (start < root->end && end > root->start) return true;

    // Check subtrees
    if (start < root->start) {
         if (avl_check_collision(root->left, start, end)) return true;
    }
    // Intervals can overlap even if start > root->start (if root extends long), 
    // but the tree is ordered by start time. 
    // A standard BST search is decent here because if end < root->start, it's definitely left.
    // However, if start > root->start, we might be right, OR we might be overlapping root.
    // The initial if check catches overlap with root.
    
    return avl_check_collision(root->right, start, end);
}

void avl_free(AVLNode* node) {
    if (node == NULL) return;
    avl_free(node->left);
    avl_free(node->right);
    free(node);
}

int avl_suggest_time(AVLNode* root, int duration, int day_start_constraint) {
    int candidate = day_start_constraint;
    // Limit search to 17:00
    while (candidate + duration <= WORK_END_MIN) {
        if (!avl_check_collision(root, candidate, candidate + duration)) {
            return candidate;
        }
        candidate += 15; // Try every 15 mins
    }
    return -1; // No slot found
}
