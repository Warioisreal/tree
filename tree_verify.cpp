#include "tree_verify.h"


static size_t djb2(size_t hash, size_t field);
static void TreeVisitPostOrder(Node_t* node, size_t* count);

tree_return_t TreeVerify(Tree_type* tree, bool check_size) {
    if (tree->root == nullptr) {
        return tree_return_t::INVALID_ROOT;
    }
    if (check_size == true && tree->size == 0) {
        return tree_return_t::EMPTY_TREE_DEL;
    }

    size_t count = 0;

    TreeVisitPostOrder(tree->root, &count);
    if (count != tree->size) {
        return tree_return_t::INVALID_SIZE;
    }

    return tree_return_t::TREE_OK;
}

//----------------------------------------------------------------------------------

tree_return_t SubTreeVerify(Node_t* node) {
    if (node == nullptr) {
        return tree_return_t::INVALID_VALUE;
    }
    if (node->value == DATA_POISON) {
        MakeRedElem(node);
        return tree_return_t::INVALID_VALUE;
    }
    if (node->hash != CalculateNodeHash(node)) {
        return tree_return_t::INVALID_HASH;
    }
    if (node->correct_childs == true && ((node->left == nullptr) != (node->right == nullptr))) {
        MakeRedElem(node);
        return tree_return_t::INVALID_CHILDS;
    }
    return tree_return_t::TREE_OK;
}

//----------------------------------------------------------------------------------

size_t CalculateNodeHash(Node_t* node) {
    if (node == nullptr) { return 0; }

    size_t struct_hash = HASH_SEED;

    struct_hash = djb2(struct_hash, (size_t)node->value);
    struct_hash = djb2(struct_hash, (size_t)node->left);
    struct_hash = djb2(struct_hash, (size_t)node->right);
    struct_hash = djb2(struct_hash, (size_t)node->allocated_node);
    struct_hash = djb2(struct_hash, (size_t)node->correct_childs);
    struct_hash = djb2(struct_hash, (size_t)node->color);
    struct_hash = djb2(struct_hash, (size_t)node->bg_color);

    return struct_hash;
}

//----------------------------------------------------------------------------------

static void TreeVisitPostOrder(Node_t* node, size_t* count) {
    if (node->left != nullptr) {
        TreeVisitPostOrder(node->left, count);
    }
    if (node->right != nullptr) {
        TreeVisitPostOrder(node->right, count);
    }
    (*count)++;
}

//----------------------------------------------------------------------------------

static size_t djb2(size_t hash, size_t field) {
    return ((hash << 5) + hash) + field;
}

