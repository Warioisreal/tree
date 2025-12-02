#include "color_lib.h"
#include "audio_lib.h"

#include "sub_func.h"
#include "tree_DB.h"


static tree_return_t TreeGetDB(Tree_type* tree, char* buffer, Node_t* node, int* position);
static Node_t* TreeReadDBRec(Tree_type* tree, char* buffer, int* position);
static tree_elem_t ReadData(char* buffer, int* count, int* position);


tree_return_t TreeMakeDB(const char* filename, Tree_type* tree) {
    #ifdef DEBUG
    TREE_VERIFY_AND_RETURN(tree, tree->root, true, "ERROR BEFORE MakeDB");
    #endif

    FILE* file = fopen(filename, "wb");

    if (file == nullptr) {
        TreeDump(tree, "DB FILE OPEN ERROR", tree_return_t::INVALID_DB_PTR);
        return tree_return_t::INVALID_DB_PTR;
    }

    char* buffer = (char*)calloc(MAX_BUFFER_DB_SIZE, sizeof(char));

    if (buffer == nullptr) {
        TreeDump(tree, "DB BUFFER CALLOC ERROR", tree_return_t::INVALID_BUFFER);
        return tree_return_t::INVALID_BUFFER;
    }

    int pos = 0;

    tree_return_t res = TreeGetDB(tree, buffer, tree->root, &pos);

    if (res == tree_return_t::TREE_OK) {
        fprintf(file, "%s", buffer);
        TreePrint(tree, "MAKE DB");

        PRINT_COLOR_VAR(GREEN, "данные успешно сохранены в файл: %s\n", filename);
        VoiceText("база данных успешно записана");
    }

    fclose(file);

    free(buffer);
    buffer = nullptr;

    return res;
}

static tree_return_t TreeGetDB(Tree_type* tree, char* buffer, Node_t* node, int* position) {
    #ifdef DEBUG
    CHECK_ERROR_AND_RETURN(tree, "ERROR IN TreeGetDB", SubTreeVerify(node), tree_return_t::TREE_OK);
    #endif

    if (buffer == nullptr) {
        TreeDump(tree, "TreeGetDB INVALID BUFFER", tree_return_t::INVALID_BUFFER);
        return tree_return_t::INVALID_BUFFER;
    }

    int res = snprintf(buffer + *position, MAX_BUFFER_DB_SIZE, "(\"%s\" ", node->value);

    if (res == 0) {
        TreeDump(tree, "DB BUFFER WRITE ERROR", tree_return_t::WRITE_BUF_ERR);
        return tree_return_t::WRITE_BUF_ERR;
    }
    *position += res;

    if (node->left == nullptr) {
        res = snprintf(buffer + *position, MAX_BUFFER_DB_SIZE, "nil ");
        if (res == 0) {
            TreeDump(tree, "DB BUFFER WRITE ERROR", tree_return_t::WRITE_BUF_ERR);
            return tree_return_t::WRITE_BUF_ERR;
        }
        *position += res;
    } else {
        TreeGetDB(tree, buffer, node->left, position);
    }
    if (node->right == nullptr) {
        res = snprintf(buffer + *position, MAX_BUFFER_DB_SIZE, "nil");
        if (res == 0) {
            TreeDump(tree, "DB BUFFER WRITE ERROR", tree_return_t::WRITE_BUF_ERR);
            return tree_return_t::WRITE_BUF_ERR;
        }
        *position += res;
    } else {
        TreeGetDB(tree, buffer, node->right, position);
    }

    res = snprintf(buffer + *position, MAX_BUFFER_DB_SIZE, ") ");
    if (res == 0) {
        TreeDump(tree, "DB BUFFER WRITE ERROR", tree_return_t::WRITE_BUF_ERR);
        return tree_return_t::WRITE_BUF_ERR;
    }
    *position += res;
    return tree_return_t::TREE_OK;
}

//----------------------------------------------------------------------------------

tree_return_t TreeReadDB(const char* filename, Tree_type* tree, char* buffer) {
    FILE* file = fopen(filename, "rb");
    fread(buffer, sizeof(char), MAX_BUFFER_DB_SIZE, file);
    fclose(file);

    int position = 0;
    tree->size = 0;
    Node_t* node = TreeReadDBRec(tree, buffer, &position);
    if (node == nullptr) {
        TreeDump(tree, "DB READ CREATE ROOT ERROR", tree_return_t::INVALID_ROOT);
        return tree_return_t::INVALID_ROOT;
    }
    tree->root = node;

    #ifdef DEBUG
    TREE_VERIFY_AND_RETURN(tree, tree->root, true, "ERROR AFTER ReadDB");
    #endif

    TreePrint(tree, "DUMP DB TREE");

    PRINT_COLOR(GREEN, "данные успешно прочитаны\n");
    VoiceText("База данных успешно прочитана");

    return tree_return_t::TREE_OK;
}

static Node_t* TreeReadDBRec(Tree_type* tree, char* buffer, int* position) {
    if (buffer[*position] == '(') {
        Node_t* node = MakeTreeElement(nullptr, false);
        tree->size++;
        (*position)++; // skip (
        if(isspace(buffer[*position]) == true) { SkipSpaces(buffer, position); }
        int count = 0;
        node->value = ReadData(buffer, &count, position);
        *position += count - 1;
        if(isspace(buffer[*position]) == true) { SkipSpaces(buffer, position); }
        node->left  = TreeReadDBRec(tree, buffer, position);
        if(isspace(buffer[*position]) == true) { SkipSpaces(buffer, position); }
        node->right = TreeReadDBRec(tree, buffer, position);
        if(isspace(buffer[*position]) == true) { SkipSpaces(buffer, position); }
        (*position)++; // skip )
        if(isspace(buffer[*position]) == true) { SkipSpaces(buffer, position); }

        node->hash = CalculateNodeHash(node);
        return node;
    }
    if (buffer[*position] == 'n') {
        *position += 3;
        return nullptr;
    }
    return nullptr;
}

static tree_elem_t ReadData(char* buffer, int* count, int* position) {
    sscanf(buffer + *position, "\"%*[^\"]\"%n", count);
    (*position)++; // skip "
    *(buffer + *position + *count - 2) = '\0';
    return buffer + *position;

}
