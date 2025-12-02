#include <ctype.h>
#include <string.h>

#include "sub_func.h"
#include "tree.h"


static size_t djb2(size_t hash, size_t field);


void CleanInput(FILE* file) {
    while (fgetc(file) != '\n') {
        continue;
    }
}

//----------------------------------------------------------------------------------

void SkipSpaces(char* buffer, int* position) {
    while (isspace(buffer[*position]) == true) {
        (*position)++;
    }
}

//----------------------------------------------------------------------------------

int CompareAnswer(const char* answer) {
    if (answer == nullptr) {return ERROR_COMP_ANSW_RES; }

    if (strncmp(answer, "yes", MAX_ANSWER_SIZE) == 0) {
        return 1;
    }

    if (strncmp(answer, "no", MAX_ANSWER_SIZE) == 0) {
        return 0;
    }

    if (strncmp(answer, "delete", MAX_ANSWER_SIZE) == 0) {
        return -1;
    }

    return ERROR_COMP_ANSW_RES;
}

//----------------------------------------------------------------------------------

void MakeGreenElem(Node_t* node) {
    node->color    = 0x00c000;
    node->bg_color = 0xa0f0a0;
    node->hash     = CalculateNodeHash(node);
}

//----------------------------------------------------------------------------------

void MakeYellowElem(Node_t* node) {
    node->color    = 0xc0c000;
    node->bg_color = 0xf0f0a0;
    node->hash     = CalculateNodeHash(node);
}

//----------------------------------------------------------------------------------

void MakeRedElem(Node_t* node) {
    if (node == nullptr) {printf("NULL\n"); return; }
    node->color    = 0xc00000;
    node->bg_color = 0xf0a0a0;
    node->hash     = CalculateNodeHash(node);
}

//----------------------------------------------------------------------------------

void MakeGreyElem(Node_t* node) {
    if (node == nullptr) {printf("NULL\n"); return; }
    node->color    = 0x808080;
    node->bg_color = 0xc0c0c0;
    node->hash     = CalculateNodeHash(node);
}

//----------------------------------------------------------------------------------

size_t CalculateNodeHash(Node_t* node) {
    if (node == nullptr) { return 0; }

    size_t struct_hash = TREE_HASH_SEED;

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

static size_t djb2(size_t hash, size_t field) {
    return ((hash << 5) + hash) + field;
}
