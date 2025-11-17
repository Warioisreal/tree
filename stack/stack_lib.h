#ifndef _STACK_LIB_H_
#define _STACK_LIB_H_

#include <ctype.h>

#include "../tree.h"

typedef Node_t* stack_elem_t;

#define ENTER_STACK_ELEMENT(var_ptr) scanf("%zu", var_ptr)
#define PRINT_STACK_ELEMENT(color, var) PRINT_COLOR_VAR(color, "%zu\n", (size_t)var)
#define PRINT_STACK_CANARY(color, var) PRINT_COLOR_VAR(color, "0x%X\n", (unsigned int)var)

#ifdef DEBUG
const size_t STRUCT_CANARY_DEFAULT = 0xbadf00d;
const stack_elem_t CANARY_DEFAULT  = 0xbadcaca;
#endif

const stack_elem_t POISON          = 0;

const size_t MAX_STACK_CAPACITY    = 1000000;

typedef struct StructInitData {
    const char* file_name = nullptr;
    int line_number       = 0;
    const char* func_name = nullptr;
    const char* obj_name  = nullptr;
} init_data_t;

typedef enum class Errors : char {
    OK                    = 0,
    STACK_POINTER_CORRUPT = 1,    // danger stack pointer
    STRUCT_CANARY_CORRUPT = 2,    // struct canary error
    SIZE_OVER_CAPACITY    = 3,    // size > capacity
    ZERO_CAPACITY         = 4,    // capacity = 0
    CAPACITY_TOO_LARGE    = 5,    // capacity exceeds maximum limit
    DATA_POINTER_CORRUPT  = 6,    // danger data pointer
    HASH_MISMATCH         = 7,    // hash mismatch
    DATA_CANARY_CORRUPT   = 8,    // data canary error
    POP_EMPTY_STACK       = 9,    // pop from empty stack
    CALLOC_FAILED         = 10,   // calloc failure
    REALLOC_FAILED        = 11    // realloc failure
} stack_error_t;

typedef struct Stack {
    #ifdef DEBUG
    size_t left_canary          = 0;
    #endif
    size_t size                 = 0;
    size_t capacity             = 0;
    stack_elem_t* data          = nullptr;
    #ifdef DEBUG
    size_t data_hash            = 0;
    init_data_t stack_info = {};
    size_t right_canary         = 0;
    size_t struct_hash          = 0;
    #endif
    stack_error_t error         = stack_error_t::OK;
} stack_type;

#endif // _STACK_LIB_H_
