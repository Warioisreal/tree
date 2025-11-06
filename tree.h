#ifndef _TREE_H_
#define _TREE_H_


typedef int tree_elem_t;


struct Node_t {
    tree_elem_t value = DATA_POISON;
    Node_t* left  = nullptr;
    Node_t* right = nullptr;
}

#endif //_TREE_H_
