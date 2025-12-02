#ifndef _TREE_DB_H_
#define _TREE_DB_H_


#include "tree.h"
#include "tree_verify.h"


#include "tree_func.h"


tree_return_t TreeMakeDB(const char* filename, Tree_type* tree);
tree_return_t TreeReadDB(const char* filename, Tree_type* tree, char* buffer);

#endif //_TREE_DB_H_
