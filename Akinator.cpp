#include <string.h>

#include "audio_lib.h"

#include "sub_func.h"
#include "Akinator.h"


static tree_return_t TreeAkinatorRec(Tree_type* tree, Node_t* node, const tree_elem_t value, FILE* file);
static tree_return_t AkinatorMakeAction(Tree_type* tree, Node_t* node, Node_t* child_node, const tree_elem_t value, FILE* file);
static tree_return_t AkinatorPredict(Tree_type* tree, Node_t* node, const tree_elem_t value, FILE* file);
static tree_return_t AkinatorAdd(Node_t* node, const tree_elem_t value, FILE* file);


tree_return_t TreeAkinator(Tree_type* tree, FILE* file) {
    printf("Что угадаем сегодня?\n");
    VoiceText("Что хотите угадать?");
    char value[MAX_ANSWER_SIZE] = "";
    fscanf(file, "%[^\n]", value);

    #ifdef DEBUG
    char error_text[MAX_DUMP_MESSAGE_SIZE] = "";

    snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR BEFORE AKINATE ELEMENT: %s", value);
    TREE_VERIFY_AND_RETURN(tree, tree->root, true, error_text);
    #endif

    return TreeAkinatorRec(tree, tree->root, value, file);
}

static tree_return_t TreeAkinatorRec(Tree_type* tree, Node_t* node, const tree_elem_t value, FILE* file) {
    if (node->left != nullptr || node->right != nullptr) {
        printf("\n%s %s\nyes/no\n", value, node->value);
        char audio_text[MAX_ANSWER_SIZE] = "";
        snprintf(audio_text, MAX_ANSWER_SIZE, "Это %s", node->value);
        VoiceText(audio_text);

        char answer_lr[MAX_ANSWER_SIZE] = "";
        int read_lr = fscanf(file, "%s", answer_lr);
        if (read_lr == 0) {
            TreeDump(tree, "ANSWER READ ERROR", tree_return_t::INVALID_ANSWER);
            return tree_return_t::INVALID_ANSWER;
        }

        if (CompareAnswer(answer_lr) == 1) {
            return AkinatorMakeAction(tree, node, node->left, value, file);
        } else
        if (CompareAnswer(answer_lr) == 0) {
            return AkinatorMakeAction(tree, node, node->right, value, file);
        } else {
            TreeDump(tree, "INCORRECT ANSWER IN AKINATOR", tree_return_t::INCORRECT_ANSW);
            return tree_return_t::INCORRECT_ANSW;
        }
    }

    return AkinatorPredict(tree, node, value, file);
}

static tree_return_t AkinatorMakeAction(Tree_type* tree, Node_t* node, Node_t* child_node, const tree_elem_t value, FILE* file) {
    if (child_node != nullptr) {
        return TreeAkinatorRec(tree, child_node, value, file);
    } else {
        printf("Я не знаю, кто это\n");

        child_node = MakeTreeElement(strdup(value), true);
        MakeGreenElem(child_node);
        MakeYellowElem(node);

        tree->size += 1;

        #ifdef DEBUG
        char error_text[MAX_DUMP_MESSAGE_SIZE] = "";

        snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR AFTER AKINATE AND ADD ELEMENT: %s", value);
        TREE_VERIFY_AND_RETURN(tree, node, true, error_text);
        #endif

        char message[MAX_DUMP_MESSAGE_SIZE] = "";
        snprintf(message, MAX_DUMP_MESSAGE_SIZE, "Add: %s", value);
        TreePrint(tree, message);

        MakeGreyElem(child_node);
        MakeGreyElem(node);

        return tree_return_t::ADD_ELEMENT;
    }
}

static tree_return_t AkinatorPredict(Tree_type* tree, Node_t* node, const tree_elem_t value, FILE* file) {
    printf("\nЯ думаю это %s\nyes/no\n", node->value);

    char answer_res[MAX_ANSWER_SIZE] = "";
    int read_res = fscanf(file, "%s", answer_res);
    if (read_res == 0) {
        TreeDump(tree, "PREDICT RESULT READ ERROR", tree_return_t::INVALID_ANSWER);
        return tree_return_t::INVALID_ANSWER;
    }

    #ifdef DEBUG
    char error_text[MAX_DUMP_MESSAGE_SIZE] = "";
    #endif

    if (CompareAnswer(answer_res) == 1) {
        printf("\nХА-ХА, это было легко!\n");

        MakeGreenElem(node);

        #ifdef DEBUG
        snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR AFTER AKINATE AND FIND ELEMENT: %s", value);
        TREE_VERIFY_AND_RETURN(tree, node, true, error_text);
        #endif

        char message[MAX_DUMP_MESSAGE_SIZE] = "";
        snprintf(message, MAX_DUMP_MESSAGE_SIZE, "Find: %s", value);
        TreePrint(tree, message);

        MakeGreyElem(node);

        return tree_return_t::TREE_OK;
    } else
    if (CompareAnswer(answer_res) == 0) {

        CHECK_ERROR_AND_RETURN(tree, "ERROR in TreeAkinatorPredict", AkinatorAdd(node, value, file), tree_return_t::ADD_ELEMENT);

        tree->size += 2;

        #ifdef DEBUG
        snprintf(error_text, MAX_DUMP_MESSAGE_SIZE, "ERROR AFTER AKINATE ELEMENT: %s", value);
        TREE_VERIFY_AND_RETURN(tree, node, true, error_text);
        #endif

        char message[MAX_DUMP_MESSAGE_SIZE] = "";
        snprintf(message, MAX_DUMP_MESSAGE_SIZE, "Add: %s", value);
        TreePrint(tree, message);

        MakeGreyElem(node);
        MakeGreyElem(node->left);
        MakeGreyElem(node->right);

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

    return tree_return_t::ADD_ELEMENT;
}
