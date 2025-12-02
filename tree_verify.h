#ifndef _TREE_VERIFY_H_
#define _TREE_VERIFY_H_

#include "define_lib.h"

#include "tree.h"
#include "tree_func.h"


#define CHECK_ERROR_AND_RETURN(tree, message, ret, result) if (ret != result) { TreeDump(tree, message, ret); return ret; }

#ifdef DEBUG
#define TREE_VERIFY_AND_RETURN(tree, node, check_size, message) BEGIN { \
    tree_return_t error = TreeVerify(tree, check_size); \
    if (error != tree_return_t::TREE_OK) { \
        SubTreeDump(tree, node, message, error); \
        return error; \
    } else { \
        error = SubTreeVerify(node); \
        if (error != tree_return_t::TREE_OK) { \
            SubTreeDump(tree, node, message, error); \
            return error; \
        } \
    } \
} END
#else
#define TREE_VERIFY_AND_RETURN(tree, node, check_size, message)
#endif

tree_return_t TreeVerify(Tree_type* tree, bool check_size);
tree_return_t SubTreeVerify(Node_t* node);

#endif //_TREE_VERIFY_H_
