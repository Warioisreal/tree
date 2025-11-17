#include <stdio.h>
#include <stdlib.h>

#include "color_lib.h"

#include "tree_func.h"
#include "logger.h"


int main (void) {

    MAKE_TREE(tree);

    FILE* file = fopen("db.txt", "rb");
    char buffer[MAX_BUFFER_DB_SIZE] = "";
    fread(buffer, sizeof(char), MAX_BUFFER_DB_SIZE, file);
    fclose(file);

    TreeReadDB(&tree, buffer);

    FILE* file_inp = fopen("input.txt", "rb");

    TreeAkinator(&tree, file_inp);

    TreeFindElement(&tree, file_inp);

    //TreeAddElement(&tree, file_inp);

    //TreeDelElement(&tree, file_inp);

    fclose(file_inp);

    TreeMakeDB("db1.txt", &tree);

    TreeDtor(&tree);

    return 0;
}
