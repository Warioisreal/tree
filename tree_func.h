#ifndef _TREE_FUNC_H_
#define _TREE_FUNC_H_

#include "tree.h"

#include "logger.h"

struct Tree_type {
    Node_t* root = nullptr;
    size_t size = 0;
    LOG* log = nullptr;
};

#define MAKE_LOG(tree_name, log_name) \
    LOG log_name = {}; \
    tree_name.log = &(log_name); \
    snprintf(tree_name.log->name, LOG_FILE_NAME_SIZE, "%s", #tree_name)

#define MAKE_TREE(tree_name) \
    Tree_type tree_name = {}; \
    MAKE_LOG(tree_name, log); \
    char full_folder_name[LOG_FOLDER_NAME_SIZE] = ""; \
    GetFullFolderName(#tree_name, full_folder_name); \
    UpdateFolder(full_folder_name); \
    TreeCtor(&tree_name)

void StartWorkWithTree(Tree_type* tree);

tree_return_t TreeCtor(Tree_type* tree);
tree_return_t TreeDtor(Tree_type* tree);

tree_return_t TreeAddElement(Tree_type* tree, FILE* file);
tree_return_t TreeDelElement(Tree_type* tree, FILE* file);

Node_t* MakeTreeElement(const tree_elem_t value, bool allocate);

void TreePrint(Tree_type* tree, const char* message);
void SubTreePrint(Tree_type* tree, Node_t* node, const char* message);
void TreeDump(Tree_type* tree, const char* message, tree_return_t error);
void SubTreeDump(Tree_type* tree, Node_t* node, const char* message, tree_return_t error);

tree_return_t TreeFindElement(Tree_type* tree, FILE* file);
tree_return_t TreeCompareElements(Tree_type* tree, FILE* file);

#endif //_TREE_FUNC_H_
