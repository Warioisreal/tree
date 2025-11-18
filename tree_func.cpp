#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stack/stack.h"
#include "color_lib.h"

#include "tree_func.h"
#include "tree_verify.h"


static tree_return_t TreeDtorRec(Node_t** node, size_t* size);

static tree_return_t TreeGetDB(Tree_type* tree, char* buffer, Node_t* node, int* position);
static Node_t* TreeReadDBRec(Tree_type* tree, char* buffer, int* position);
static tree_elem_t ReadData(char* buffer, int* count, int* position);

static Node_t* TreeAddElementRec(Node_t* node, const tree_elem_t value, FILE* file);
static Node_t* TreeDelElementRec(Node_t** node, size_t* size, FILE* file);

static Node_t* MakeTreeElement(const tree_elem_t value, bool allocate);

static tree_return_t TreeAkinatorRec(Tree_type* tree, Node_t* node, const tree_elem_t value, FILE* file);
static tree_return_t AkinatorMakeAction(Tree_type* tree, Node_t* node, Node_t* child_node, const tree_elem_t value, FILE* file);
static tree_return_t AkinatorPredict(Tree_type* tree, Node_t* node, const tree_elem_t value, FILE* file);
static tree_return_t AkinatorAdd(Node_t* node, const tree_elem_t value, FILE* file);

static void CleanInput(FILE* file);
static void SkipSpaces(char* buffer, int* position);

static Node_t* TreeFindElementRec(stack_type* stack, Node_t* node, const tree_elem_t value);

static void TreeOutQuestionNode(Node_t* node, char* buffer, int* position);

static void WriteNodeToBuffer(char* buffer, int* position, stack_elem_t is_left_child, Node_t* node1, Node_t* node2);
static void NodeUpdate(Node_t** node, stack_elem_t is_left_child);


tree_return_t TreeCtor(Tree_type* tree) {
    FILE* file_ = nullptr;
    StartLog(&file_, tree->log->name);
    if (file_ == nullptr) {
        TreeDump(tree, "LOG FILE OPEN ERROR", tree_return_t::INVALID_LOG);
        return tree_return_t::INVALID_LOG;
    }
    tree->log->file_log = file_;
    tree->log->dump_count = 1;

    tree->root = MakeTreeElement("NOTHING", false);

    MakeGreenElem(tree->root);

    tree->root->hash = CalculateNodeHash(tree->root);

    tree->size = 1;

    TREE_VERIFY_AND_RETURN(tree, tree->root, true, "ERROR IN Ctor");

    TreePrint(tree, "Ctor");

    MakeGreyElem(tree->root);

    tree->root->hash = CalculateNodeHash(tree->root);

    return tree_return_t::TREE_OK;
}

//----------------------------------------------------------------------------------

tree_return_t TreeDtor(Tree_type* tree) {
    TREE_VERIFY_AND_RETURN(tree, tree->root, false, "ERROR IN Dtor");

    TreePrint(tree, "Dtor");

    FinishLog(&(tree->log->file_log));
    return TreeDtorRec(&(tree->root), &(tree->size));
}

static tree_return_t TreeDtorRec(Node_t** node, size_t* size) {
    if (*node == nullptr) {
        printf("NODE POINTER ERROR IN TreeDtorRec\n");
        return tree_return_t::INVALID_NODE;
    }

    if ((*node)->left != nullptr) {
        TreeDtorRec(&((*node)->left), size);
    }
    if ((*node)->right != nullptr) {
        TreeDtorRec(&((*node)->right), size);
    }
    if ((*node)->allocated_node == true) {
        free(const_cast<char*>((*node)->value));
    }
    (*node)->value = nullptr;
    free(*node);
    *node = nullptr;
    (*size)--;
    return tree_return_t::TREE_OK;
}

//----------------------------------------------------------------------------------

tree_return_t TreeAddElement(Tree_type* tree, FILE* file) {
    char value[MAX_ANSWER_SIZE] = "";
    fscanf(file, "\"%[^\"]", value);

    char error_text[MAX_DUMP_MESSAGE_SIZE] = "";

    snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR BEFORE ADD ELEMENT: %s", value);
    TREE_VERIFY_AND_RETURN(tree, tree->root, true, error_text);

    Node_t* node = TreeAddElementRec(tree->root, value, file);

    if (node == nullptr) {
        SubTreeDump(tree, node, "INVALID NODE POINTER", tree_return_t::INVALID_NODE);
        return tree_return_t::INVALID_NODE;
    }

    node->correct_childs = !node->correct_childs;
    tree->size++;

    snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR AFTER ADD ELEMENT: %s", value);
    TREE_VERIFY_AND_RETURN(tree, node, true, error_text);

    char message[MAX_DUMP_MESSAGE_SIZE] = "";
    snprintf(message, MAX_DUMP_MESSAGE_SIZE, "Add: %s", value);
    TreePrint(tree, message);

    MakeGreyElem(node);
    if (node->left  != nullptr) { MakeGreyElem(node->left);  }
    if (node->right != nullptr) { MakeGreyElem(node->right); }

    return tree_return_t::TREE_OK;
}

static Node_t* TreeAddElementRec(Node_t* node, const tree_elem_t value, FILE* file) {
    if (node == nullptr) { return nullptr; }

    printf("\n%s\nyes/no\n", node->value);
    char answer[MAX_ANSWER_SIZE] = "";
    int read = fscanf(file, "%s", answer);
    if (read == 0) {
        printf("read answer error\n");
        return nullptr;
    }

    if (strncmp(answer, "yes", MAX_ANSWER_SIZE) == 0) {
        if (node->left != nullptr) {
            TreeAddElementRec(node->left, value, file);

            return node->left;
        } else {
            node->left = MakeTreeElement(strdup(value), true);
            MakeYellowElem(node);
            node->hash = CalculateNodeHash(node);
            MakeGreenElem(node->left);

            return node;
        }
    } else
    if (strncmp(answer, "no", MAX_ANSWER_SIZE) == 0) {
        if (node->right != nullptr) {
            TreeAddElementRec(node->right, value, file);

            return node->right;
        } else {
            node->right = MakeTreeElement(strdup(value), true);
            MakeYellowElem(node);
            node->hash = CalculateNodeHash(node);
            MakeGreenElem(node->right);

            return node;
        }
    } else {
        printf("invalid answer\n");

        return nullptr;
    }
}

//----------------------------------------------------------------------------------

tree_return_t TreeDelElement(Tree_type* tree, FILE* file) {
    char value[MAX_ANSWER_SIZE] = "";
    fscanf(file, "\"%[^\"]", value);

    char error_text[MAX_DUMP_MESSAGE_SIZE] = "";

    snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR BEFORE DELETE ELEMENT: %s", value);
    TREE_VERIFY_AND_RETURN(tree, tree->root, true, error_text);

    Node_t* node = TreeDelElementRec(&(tree->root), &(tree->size), file);
    if (node == nullptr) {
        SubTreeDump(tree, node, "INVALID NODE POINTER", tree_return_t::INVALID_NODE);
        return tree_return_t::INVALID_NODE;
    }
    node->correct_childs = !node->correct_childs;
    MakeYellowElem(node);

    snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR BEFORE DELETE ELEMENT: %s", value);
    TREE_VERIFY_AND_RETURN(tree, node, false, error_text);

    char message[MAX_DUMP_MESSAGE_SIZE] = "";
    snprintf(message, MAX_DUMP_MESSAGE_SIZE, "Delete: %s", value);

    TreePrint(tree, message);

    MakeGreyElem(node);

    return tree_return_t::TREE_OK;
}

static Node_t* TreeDelElementRec(Node_t** node, size_t* size, FILE* file) {
    if (*node == nullptr) { return nullptr; }

    printf("\n%s\nyes/no/delete\n", (*node)->value);
    char answer[MAX_ANSWER_SIZE] = "";
    int read = fscanf(file, "%s", answer);
    if (read == 0) {
        printf("read answer error\n");
        return nullptr;
    }

    if (strncmp(answer, "yes", MAX_ANSWER_SIZE) == 0) {
        TreeDelElementRec(&((*node)->left), size, file);
        (*node)->hash = CalculateNodeHash(*node);

        return (*node);
    } else
    if (strncmp(answer, "no", MAX_ANSWER_SIZE) == 0) {
        TreeDelElementRec(&((*node)->right), size, file);
        (*node)->hash = CalculateNodeHash(*node);

        return (*node);
    } else
    if (strncmp(answer, "delete", MAX_ANSWER_SIZE) == 0){
        TreeDtorRec(node, size);

        return nullptr;
    } else {
        printf("invalid answer\n");

        return nullptr;
    }
}

//----------------------------------------------------------------------------------

void TreePrint(Tree_type* tree, const char* message) {
    SubTreePrint(tree, tree->root, message);
}

void SubTreePrint(Tree_type* tree, Node_t* node, const char* message) {
    SubTreeDump(tree, node, message, tree_return_t::TREE_OK);
}

void TreeDump(Tree_type* tree, const char* message, tree_return_t error) {
    SubTreeDump(tree, tree->root, message, error);
}

void SubTreeDump(Tree_type* tree, Node_t* node, const char* message, tree_return_t error) {
    if (error != tree_return_t::TREE_OK) {
        PRINT_COLOR(CYAN, "\n============TREE DUMP=============\n");
        PRINT_COLOR_VAR(RED, "ERROR: %s\n", TreeErrorsArray[static_cast <int>(error)]);
    }

    switch(error) {
        case tree_return_t::TREE_OK:
        case tree_return_t::INVALID_ANSWER:
        case tree_return_t::INCORRECT_ANSW:
            GoLog(tree->root, tree->size, node, message, tree->log);
            break;
        case tree_return_t::EMPTY_TREE_DEL:
            printf("| ___ROOT___ | ___SIZE___ | ___LOG___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)tree->root);
            printf("|");
            PRINT_COLOR_VAR(YELLOW, " %10zu ", tree->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %9zX ", (size_t)tree->log);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", tree->log->dump_count);
            GoLog(tree->root, tree->size, node, message, tree->log);
            break;
        case tree_return_t::INVALID_SIZE:
            printf("| ___ROOT___ | ___SIZE___ | ___LOG___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)tree->root);
            printf("|");
            PRINT_COLOR_VAR(RED, " %10zu ", tree->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %9zX ", (size_t)tree->log);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", tree->log->dump_count);
            GoLog(tree->root, tree->size, node, message, tree->log);
            break;
        case tree_return_t::INVALID_LOG:
            printf("| ___ROOT___ | ___SIZE___ | ___LOG___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)tree->root);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", tree->size);
            printf("|");
            PRINT_COLOR_VAR(RED, " %9zX ", (size_t)tree->log);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", tree->log->dump_count);
            GoLog(tree->root, tree->size, node, message, tree->log);
            break;
        case tree_return_t::INVALID_ROOT:
            printf("| ___ROOT___ | ___SIZE___ | ___LOG___ |\n");
            printf("|");
            PRINT_COLOR_VAR(RED, " %10zX ", (size_t)tree->root);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", tree->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %9zX ", (size_t)tree->log);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", tree->log->dump_count);
            GoLog(tree->root, tree->size, node, message, tree->log);
            break;
        case tree_return_t::ADD_ELEMENT:
        case tree_return_t::DELETE_SUBTREE:
            printf("| ___ROOT___ | ___SIZE___ | ___LOG___ | ______UPD NODE______ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)tree->root);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", tree->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %9zX ", (size_t)tree->log);
            printf("|");
            PRINT_COLOR_VAR(YELLOW, " %20s ", node->value);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", tree->log->dump_count);
            GoLog(tree->root, tree->size, node, message, tree->log);
            break;
        case tree_return_t::INVALID_CHILDS:
        case tree_return_t::INVALID_HASH:
        case tree_return_t::INVALID_VALUE:
            printf("| ___ROOT___ | ___SIZE___ | ___LOG___ | ___INCORRECT NODE___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)tree->root);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", tree->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %9zX ", (size_t)tree->log);
            printf("|");
            PRINT_COLOR_VAR(RED, " %20s ", node->value);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", tree->log->dump_count);
            GoLog(tree->root, tree->size, node, message, tree->log);
            break;
        case tree_return_t::INVALID_NODE:
            printf("| ___ROOT___ | ___SIZE___ | ___LOG___ | ___INCORRECT NODE___ |\n");
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zX ", (size_t)tree->root);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %10zu ", tree->size);
            printf("|");
            PRINT_COLOR_VAR(GREEN, " %9zX ", (size_t)tree->log);
            printf("|");
            PRINT_COLOR_VAR(RED, " %10zX ", (size_t)node);
            printf("|");
            printf("\n\n");
            printf("DUMP NUMBER: %zu\n", tree->log->dump_count);
            GoLog(tree->root, tree->size, node, message, tree->log);
            break;
        case tree_return_t::INVALID_BUFFER:
            PRINT_COLOR(RED, "Ошибка буфера в работе с BD\n");
            break;
        case tree_return_t::WRITE_BUF_ERR:
            PRINT_COLOR(RED, "Ошибка записи в буфер\n");
            break;
        case tree_return_t::INVALID_DB_PTR:
            PRINT_COLOR(RED, "Ошибка аллокации буфера\n");
            break;
        default:
            printf("UNKNOWN RETURN\n");
    }

    if (error != tree_return_t::TREE_OK) { PRINT_COLOR(CYAN, "==================================\n\n"); }
}

//----------------------------------------------------------------------------------

tree_return_t TreeMakeDB(const char* filename, Tree_type* tree) {
    TREE_VERIFY_AND_RETURN(tree, tree->root, true, "ERROR BEFORE MakeDB");

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
    }

    fclose(file);

    free(buffer);
    buffer = nullptr;

    return res;
}

static tree_return_t TreeGetDB(Tree_type* tree, char* buffer, Node_t* node, int* position) {
    CHECK_ERROR_AND_RETURN(tree, "ERROR IN TreeGetDB", SubTreeVerify(node), tree_return_t::TREE_OK);

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

tree_return_t TreeReadDB(Tree_type* tree, char* buffer) {
    int position = 0;
    tree->size = 0;
    Node_t* node = TreeReadDBRec(tree, buffer, &position);
    if (node == nullptr) {
        TreeDump(tree, "DB READ CREATE ROOT ERROR", tree_return_t::INVALID_ROOT);
        return tree_return_t::INVALID_ROOT;
    }
    tree->root = node;

    TREE_VERIFY_AND_RETURN(tree, tree->root, true, "ERROR AFTER ReadDB");

    TreePrint(tree, "DUMP DB TREE");

    return tree_return_t::TREE_OK;
}

static Node_t* TreeReadDBRec(Tree_type* tree, char* buffer, int* position) {
    if (buffer[*position] == '(') {
        Node_t* node = MakeTreeElement("", false);
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

//----------------------------------------------------------------------------------

static Node_t* MakeTreeElement(const tree_elem_t value, bool allocate) {
    Node_t* buf = (Node_t*)calloc(1, sizeof(Node_t));
    if (buf == nullptr) {
        printf("node allocate error\n");
        return buf;
    }

    buf->value = value;
    buf->left  = nullptr;
    buf->right = nullptr;
    buf->allocated_node = allocate;
    buf->color    = 0x808080;
    buf->bg_color = 0xc0c0c0;

    buf->hash = CalculateNodeHash(buf);

    return buf;
}

//----------------------------------------------------------------------------------

tree_return_t TreeAkinator(Tree_type* tree, FILE* file) {
    printf("Что угадаем сегодня?\n");
    char value[MAX_ANSWER_SIZE] = "";
    fscanf(file, "%[^\n]", value);

    char error_text[MAX_DUMP_MESSAGE_SIZE] = "";

    snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR BEFORE AKINATE ELEMENT: %s", value);
    TREE_VERIFY_AND_RETURN(tree, tree->root, true, error_text);

    return TreeAkinatorRec(tree, tree->root, value, file);
}

static tree_return_t TreeAkinatorRec(Tree_type* tree, Node_t* node, const tree_elem_t value, FILE* file) {
    if (node->left != nullptr || node->right != nullptr) {
        printf("\nЭто %s\nyes/no\n", node->value);

        char answer_lr[MAX_ANSWER_SIZE] = "";
        int read_lr = fscanf(file, "%s", answer_lr);
        if (read_lr == 0) {
            TreeDump(tree, "ANSWER READ ERROR", tree_return_t::INVALID_ANSWER);
            return tree_return_t::INVALID_ANSWER;
        }

        if (strncmp(answer_lr, "yes", MAX_ANSWER_SIZE) == 0) {
            return AkinatorMakeAction(tree, node, node->left, value, file);
        } else
        if (strncmp(answer_lr, "no", MAX_ANSWER_SIZE) == 0) {
            return AkinatorMakeAction(tree, node, node->right, value, file);
        } else {
            TreeDump(tree, "INCORRECT ANSWER IN AKINATOR", tree_return_t::INCORRECT_ANSW);
            return tree_return_t::INCORRECT_ANSW;
        }
    }

    return AkinatorPredict(tree, node, value, file);
}

static tree_return_t AkinatorMakeAction(Tree_type* tree, Node_t* node, Node_t* child_node, const tree_elem_t value, FILE* file) {
    char error_text[MAX_DUMP_MESSAGE_SIZE] = "";

    if (child_node != nullptr) {
        return TreeAkinatorRec(tree, child_node, value, file);
    } else {
        printf("Я не знаю, кто это\n");

        child_node = MakeTreeElement(strdup(value), true);
        MakeGreenElem(child_node);
        MakeYellowElem(node);
        node->hash = CalculateNodeHash(node);

        tree->size += 1;

        snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR AFTER AKINATE AND ADD ELEMENT: %s", value);
        TREE_VERIFY_AND_RETURN(tree, node, true, error_text);

        char message[MAX_DUMP_MESSAGE_SIZE] = "";
        snprintf(message, MAX_DUMP_MESSAGE_SIZE, "Add: %s", value);
        TreePrint(tree, message);

        MakeGreyElem(child_node);
        MakeGreyElem(node);
        node->hash = CalculateNodeHash(node);

        return tree_return_t::ADD_ELEMENT;
    }
}

static tree_return_t AkinatorPredict(Tree_type* tree, Node_t* node, const tree_elem_t value, FILE* file) {
    char error_text[MAX_DUMP_MESSAGE_SIZE] = "";

    printf("\nЯ думаю это %s\nyes/no\n", node->value);

    char answer_res[MAX_ANSWER_SIZE] = "";
    int read_res = fscanf(file, "%s", answer_res);
    if (read_res == 0) {
        TreeDump(tree, "PREDICT RESULT READ ERROR", tree_return_t::INVALID_ANSWER);
        return tree_return_t::INVALID_ANSWER;
    }

    if (strncmp(answer_res, "yes", MAX_ANSWER_SIZE) == 0) {
        printf("\nХА-ХА, это было легко!\n");

        MakeGreenElem(node);
        node->hash = CalculateNodeHash(node);

        snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR AFTER AKINATE AND FIND ELEMENT: %s", value);
        TREE_VERIFY_AND_RETURN(tree, node, true, error_text);

        char message[MAX_DUMP_MESSAGE_SIZE] = "";
        snprintf(message, MAX_DUMP_MESSAGE_SIZE, "Find: %s", value);
        TreePrint(tree, message);

        MakeGreyElem(node);
        node->hash = CalculateNodeHash(node);

        return tree_return_t::TREE_OK;
    } else
    if (strncmp(answer_res, "no", MAX_ANSWER_SIZE) == 0) {

        CHECK_ERROR_AND_RETURN(tree, "ERROR in TreeAkinatorPredict", AkinatorAdd(node, value, file), tree_return_t::ADD_ELEMENT);

        tree->size += 2;
        snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR AFTER AKINATE ELEMENT: %s", value);
        TREE_VERIFY_AND_RETURN(tree, node, true, error_text);

        char message[MAX_DUMP_MESSAGE_SIZE] = "";
        snprintf(message, MAX_DUMP_MESSAGE_SIZE, "Add: %s", value);
        TreePrint(tree, message);

        MakeGreyElem(node);
        MakeGreyElem(node->left);
        MakeGreyElem(node->right);
        node->hash = CalculateNodeHash(node);

        return tree_return_t::ADD_ELEMENT;
    } else {
        TreeDump(tree, "INCORRECT ANSWER IN AKINATOR", tree_return_t::INCORRECT_ANSW);
        return tree_return_t::INCORRECT_ANSW;
    }
}

static tree_return_t AkinatorAdd(Node_t* node, const tree_elem_t value, FILE* file) {
    printf(
        "\nЯ не знаю, кто это\n"
        "\nЗапишите вопрос, продолжив фразу:\n"
        "Чем %s отличается от %s?\n"
        "Он ...\n", value, node->value);
    char answer_qst[MAX_ANSWER_SIZE] = "";
    CleanInput(file);
    int read_qst = fscanf(file, "%[^\n]", answer_qst);
    CleanInput(file);
    if (read_qst == 0) { return tree_return_t::INVALID_ANSWER; }

    node->right = MakeTreeElement(node->value, node->allocated_node);
    node->left  = MakeTreeElement(strdup(value), true);

    node->value = strdup(answer_qst);
    node->allocated_node = true;

    MakeYellowElem(node->right);
    MakeGreenElem(node->left);
    MakeGreenElem(node);

    node->hash = CalculateNodeHash(node);

    return tree_return_t::ADD_ELEMENT;
}

//----------------------------------------------------------------------------------

static void CleanInput(FILE* file) {
    while (fgetc(file) != '\n') {
        continue;
    }
}

//----------------------------------------------------------------------------------

static void SkipSpaces(char* buffer, int* position) {
    while (isspace(buffer[*position]) == true) {
        (*position)++;
    }
}

//----------------------------------------------------------------------------------

tree_return_t TreeFindElement(Tree_type* tree, FILE* file) {
    printf("\nЧто будем искать?\n");

    char value[MAX_ANSWER_SIZE] = "";
    fscanf(file, "%[^\n]", value);
    CleanInput(file);

    char error_text[MAX_DUMP_MESSAGE_SIZE] = "";

    snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR BEFORE FINDING ELEMENT: %s", value);
    TREE_VERIFY_AND_RETURN(tree, tree->root, true, error_text);

    MAKE_STACK(stack);

    if (TreeFindElementRec(&stack, tree->root, value) != nullptr) {
        char buffer[MAX_ANSWER_SIZE] = "";
        const int offset = snprintf(buffer, MAX_ANSWER_SIZE, "say ");

        int position = offset;

        position += snprintf(buffer + position, MAX_ANSWER_SIZE, "%s - ", value);

        stack_elem_t is_left_child = -1;

        Node_t* node = tree->root;

        while (stack.size > 0) {
            StackPop(&stack, &is_left_child);

            if (is_left_child == 0) { position += snprintf(buffer + position, MAX_ANSWER_SIZE, "не "); }

            TreeOutQuestionNode(node, buffer, &position);
            NodeUpdate(&node, is_left_child);

            if (stack.size > 0) { position += snprintf(buffer + position, MAX_ANSWER_SIZE, ", а ещё "); }
        }

        printf("\n%s\n", buffer + offset);
        system(buffer);
    } else {
        printf("NOT IN TREE ELEMENT: %s\n", value);
    }

    return tree_return_t::TREE_OK;
}

static Node_t* TreeFindElementRec(stack_type* stack, Node_t* node, const tree_elem_t value) {
    if (node->left != nullptr) {
        Node_t* ret1 = TreeFindElementRec(stack, node->left, value);
        if (ret1 != nullptr) {
            StackPush(stack, 1);
            return ret1;
        }
    }
    if (node->right != nullptr) {
        Node_t* ret2 = TreeFindElementRec(stack, node->right, value);
        if (ret2 != nullptr) {
            StackPush(stack, 0);
            return ret2;
        }
    }
    if (strcmp(node->value, value) == 0) { return node; }
    return nullptr;
}

static void TreeOutQuestionNode(Node_t* node, char* buffer, int* position) {
    for (int pos = 0; node->value[pos] != '?'; pos++) {
        *position += snprintf(buffer + *position, MAX_ANSWER_SIZE, "%c", node->value[pos]);
    }
}

//----------------------------------------------------------------------------------

tree_return_t TreeCompareElements(Tree_type* tree, FILE* file) {
    printf("\nЧто будем сравнивать?\n");

    char value1[MAX_ANSWER_SIZE] = "";
    char value2[MAX_ANSWER_SIZE] = "";

    fscanf(file, "%[^\n]", value1); CleanInput(file);
    fscanf(file, "%[^\n]", value2); CleanInput(file);

    char error_text[MAX_DUMP_MESSAGE_SIZE] = "";

    snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR BEFORE COMPARE ELEMENTS: %s | %s", value1, value2);
    TREE_VERIFY_AND_RETURN(tree, tree->root, true, error_text);

    MAKE_STACK(stack1);
    MAKE_STACK(stack2);

    Node_t* ret1 = TreeFindElementRec(&stack1, tree->root, value1);
    Node_t* ret2 = TreeFindElementRec(&stack2, tree->root, value2);

    if (ret1 != nullptr && ret2 != nullptr ) {
        char buffer[MAX_ANSWER_SIZE]  = "";
        char buffer1[MAX_ANSWER_SIZE] = "";
        char buffer2[MAX_ANSWER_SIZE] = "";

        const int offset  = snprintf(buffer,  MAX_ANSWER_SIZE, "say ");

        int position  = offset;
        int position1 = 0;
        int position2 = 0;

        if (stack1.data[stack1.size - 1] == stack2.data[stack2.size - 1]) {
            position  += snprintf(buffer + position, MAX_ANSWER_SIZE, "%s и %s - ", value1, value2);
            position1 += snprintf(buffer1, MAX_ANSWER_SIZE, ", однако %s - ", value1);
            position2 += snprintf(buffer2, MAX_ANSWER_SIZE, ", а %s - ", value2);
        } else {
            position  += snprintf(buffer + position, MAX_ANSWER_SIZE, "%s и %s не имеют ничего общего", value1, value2);
            position1 += snprintf(buffer1, MAX_ANSWER_SIZE, ", ведь %s - ", value1);
            position2 += snprintf(buffer2, MAX_ANSWER_SIZE, ", а %s - ", value2);
        }

        stack_elem_t is_left_child1 = -1;
        stack_elem_t is_left_child2 = -1;

        Node_t* node1 = tree->root;
        Node_t* node2 = tree->root;

        bool equal = true;

        while (stack1.size + stack2.size > 0) {
            if (stack1.size > 0) { StackPop(&stack1, &is_left_child1); }
            if (stack2.size > 0) { StackPop(&stack2, &is_left_child2); }

            if (equal == true && is_left_child1 != is_left_child2) {
                equal = false;
                node2 = node1;
            }
            if (equal == true) {
                WriteNodeToBuffer(buffer,  &position,  is_left_child1, node1, node2);
                NodeUpdate(&node1, is_left_child1);
            } else {
                WriteNodeToBuffer(buffer1, &position1, is_left_child1, node1, node2);
                WriteNodeToBuffer(buffer2, &position2, is_left_child2, node2, node1);
                if (node1 != nullptr && node1->right != nullptr) { NodeUpdate(&node1, is_left_child1); }
                if (node2 != nullptr && node2->right != nullptr) { NodeUpdate(&node2, is_left_child2); }
            }
        }
        if (ret1 != ret2) { position += snprintf(buffer + position, MAX_ANSWER_SIZE, "%s%s", buffer1, buffer2); }

        printf("\n%s\n", buffer + offset);
        system(buffer);
    } else {
        if (ret1 == nullptr) { printf("NOT IN TREE ELEMENT: %s\n", value1); }
        if (ret2 == nullptr) { printf("NOT IN TREE ELEMENT: %s\n", value2); }
    }

    return tree_return_t::TREE_OK;
}

static void WriteNodeToBuffer(char* buffer, int* position, stack_elem_t is_left_child, Node_t* node1, Node_t* node2) {
    if (node1 != node2 && node1 != nullptr && node1->right != nullptr) {
        *position += snprintf(buffer + *position, MAX_ANSWER_SIZE, ", а ещё ");
    }

    if (is_left_child == 0) { *position += snprintf(buffer + *position, MAX_ANSWER_SIZE, "не "); }

    if (node1 != nullptr && node1->right != nullptr) {
        TreeOutQuestionNode(node1, buffer, position);
    }
}

static void NodeUpdate(Node_t** node, stack_elem_t is_left_child) {
    if (is_left_child == 1) { *node = (*node)->left; }
    else {                    *node = (*node)->right; }
}
