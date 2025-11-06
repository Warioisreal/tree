#include "tree.h"



void TreeCtor(Node_t* root, const tree_elem_t value) {
    buf = (Node_t*)calloc(1, sizeof(Node_t));
    if (buf == nullptr) { return; }
    root->left = buf;
    (root->left)->value = value;
    (root->left)->left  = nullptr;
    (root->left)->right = nullptr;
}


void TreeAddElement(Node_t* node, const tree_elem_t value) {
    if (node->value <= value) {
        if (node->left != nullptr) {
            AddElement(node->left, value);
        } else {
            buf = (Node_t*)calloc(1, sizeof(Node_t));
            if (buf == nullptr) { return; }
            node->left = buf;
            (node->left)->value = value;
            (node->left)->left  = nullptr;
            (node->left)->right = nullptr;
        }
    } else {
        if (node->right != nullptr) {
            AddElement(node->right, value);
        } else {
            buf = (Node_t*)calloc(1, sizeof(Node_t));
            if (buf == nullptr) { return; }
            node->right = buf;
            (node->left)->value = value;
            (node->left)->left  = nullptr;
            (node->left)->right = nullptr;
    }
}

void TreeDump(Node_t* node) {
    printf("(");
    if (node->left != nullptr) {
        TreeDump(node->left);
    }

    printf("%d", node->value);

    if (node->right != nullptr) {
        TreeDump(node->right);
    }
    printf(")");
}
