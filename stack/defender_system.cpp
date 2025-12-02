#include <stdio.h>

#include "color_lib.h"
#include "stack_lib.h"

#include "defender_system.h"

#ifdef DEBUG
static size_t djb2(size_t hash, size_t field);

static result DumpCallInfo(call_data_t* call_info);
#endif

static result DumpStackInfo(stack_type* stack);
static result DumpStackFields(stack_type* stack);
static result DumpStackData(stack_type* stack);

#ifdef DEBUG
static inline stack_elem_t GetLeftDataCanary(stack_type* stack);
static inline stack_elem_t GetRightDataCanary(stack_type* stack);
#endif

stack_error_t CheckStackIntegrity(stack_type* stack) {
    // check stack pointer
    if ((size_t)stack < 0x1000) { return stack_error_t::STACK_POINTER_CORRUPT; }

    #ifdef DEBUG
    // check struct canary
    if (stack->left_canary  != STRUCT_CANARY_DEFAULT ||
        stack->right_canary != STRUCT_CANARY_DEFAULT) { return stack_error_t::STRUCT_CANARY_CORRUPT; }
    #endif

    // check stack capacity
    if (stack->capacity == 0) { return stack_error_t::ZERO_CAPACITY; }
    if (stack->capacity > MAX_STACK_CAPACITY) { return stack_error_t::CAPACITY_TOO_LARGE; }

    // check data pointer
    if ((size_t)stack->data < 0x1000) { return stack_error_t::DATA_POINTER_CORRUPT; }

    // check struct size
    if (stack->size > stack->capacity) { return stack_error_t::SIZE_OVER_CAPACITY; }

    #ifdef DEBUG
    // check data canary
    if (GetLeftDataCanary(stack) != CANARY_DEFAULT ||
        GetRightDataCanary(stack) != CANARY_DEFAULT) { return stack_error_t::DATA_CANARY_CORRUPT; }

    // calculating current struct_hash and data_hash
    size_t current_struct_hash = CalculateStructHash(stack);
    size_t current_data_hash = CalculateDataHash(stack);

    // check struct_hash
    if (current_struct_hash != stack->struct_hash) { return stack_error_t::HASH_MISMATCH; }

    // check data_hash
    if (current_data_hash != stack->data_hash) { return stack_error_t::HASH_MISMATCH; }
    #endif

    return stack_error_t::OK;
}

//----------------------------------------------------------------------------------
#ifdef DEBUG
void StackDump(stack_type* stack, call_data_t* call_info, const char* message) {
    if (message != nullptr) { PRINT_COLOR_VAR(RED, "%s\n", message); }

    if (DumpCallInfo(call_info) == result::STOP) { return; }
    if (DumpStackInfo(stack)    == result::STOP) { return; }
    if (DumpStackFields(stack)  == result::STOP) { return; }

    DumpStackData(stack);
}
#else
void StackDump(stack_type* stack, const char* message) {
    if (message != nullptr) { PRINT_COLOR_VAR(RED, "%s\n", message); }

    if (DumpStackInfo(stack)    == result::STOP) { return; }
    if (DumpStackFields(stack)  == result::STOP) { return; }

    DumpStackData(stack);
}
#endif
//----------------------------------------------------------------------------------
#ifdef DEBUG
size_t CalculateStructHash(stack_type* stack) {
    if (stack == nullptr) { return 0; }

    size_t struct_hash = HASH_SEED;

    // left struct canary
    struct_hash = djb2(struct_hash, stack->left_canary);

    // stack size
    struct_hash = djb2(struct_hash, stack->size);

    // stack capacity
    struct_hash = djb2(struct_hash, stack->capacity);

    // data pointer
    struct_hash = djb2(struct_hash, (size_t)stack->data);

    // right struct canary
    struct_hash = djb2(struct_hash, stack->right_canary);

    return struct_hash;
}

//----------------------------------------------------------------------------------

size_t CalculateDataHash(stack_type* stack) {
    if (stack == nullptr) return 0;

    size_t data_hash = HASH_SEED;

    // left data canary
    data_hash = djb2(data_hash, (size_t)GetLeftDataCanary(stack));

    for (size_t pos = 0; pos < stack->capacity; pos++) {
        data_hash = djb2(data_hash, (size_t)stack->data[pos]);
    }

    // right data canary
    data_hash = djb2(data_hash, (size_t)GetRightDataCanary(stack));

    return data_hash;
}

//----------------------------------------------------------------------------------

static size_t djb2(size_t hash, size_t field) {
    return ((hash << 5) + hash) + field;
}

//----------------------------------------------------------------------------------

static result DumpCallInfo(call_data_t* call_info) {
    printf("\nerror_in_file: ");
    PRINT_COLOR_VAR(YELLOW, "%s", call_info->file_name);
    printf("  line: ");
    PRINT_COLOR_VAR(YELLOW, "%d", call_info->line_number);
    printf("  function: ");
    PRINT_COLOR_VAR(YELLOW, "%s", call_info->func_name);
    printf("\n\n");

    return result::CONTINUE;
}
#endif
//----------------------------------------------------------------------------------

static result DumpStackInfo(stack_type* stack) {
    if (stack == nullptr) {
        printf("STACK_POINTER: ");
        PRINT_COLOR(RED, "CORRUPTED\n");
        printf("    stack = ");
        PRINT_COLOR_VAR(RED, "%p", stack);
        if (stack == nullptr) {
            printf(" (null pointer)\n");
        } else {
            printf(" (suspicious value: %p)\n", stack);
        }
        return result::STOP;
    } else {
        printf("STACK_POINTER: ");
        PRINT_COLOR_VAR(BLUE, "%p\n", stack);
        #ifdef DEBUG
        printf("  stack_object_name: ");
        PRINT_COLOR_VAR(YELLOW, "%s", stack->stack_info.obj_name);
        printf("  create_in_file: ");
        PRINT_COLOR_VAR(YELLOW, "%s", stack->stack_info.file_name);
        printf("  line: ");
        PRINT_COLOR_VAR(YELLOW, "%d", stack->stack_info.line_number);
        printf("  function: ");
        PRINT_COLOR_VAR(YELLOW, "%s", stack->stack_info.func_name);
        #endif
        printf("\n");
    }
    return result::CONTINUE;
}

//----------------------------------------------------------------------------------

static result DumpStackFields(stack_type* stack) {

    stack_error_t error = stack->error;

    switch(error) {
        case stack_error_t::STRUCT_CANARY_CORRUPT:
            #ifdef DEBUG
            printf("  STRUCT_CANARY: ");
            PRINT_COLOR(RED, "CORRUPTED\n");
            printf("    left_canary = ");
            if (stack->left_canary != STRUCT_CANARY_DEFAULT) {
                PRINT_COLOR_VAR(RED, "0x%zu", stack->left_canary);
                printf(" (expected: ");
                PRINT_COLOR_VAR(GREEN, "0x%zu", STRUCT_CANARY_DEFAULT);
                printf(")\n");
            } else {
                PRINT_COLOR_VAR(GREEN, "0x%zu\n", stack->left_canary);
            }
            printf("    right_canary = ");
            if (stack->right_canary != STRUCT_CANARY_DEFAULT) {
                PRINT_COLOR_VAR(RED, "0x%zu", stack->right_canary);
                printf(" (expected: ");
                PRINT_COLOR_VAR(GREEN, "0x%zu", STRUCT_CANARY_DEFAULT);
                printf(")\n");
            } else {
                PRINT_COLOR_VAR(GREEN, "0x%zu\n", stack->right_canary);
            }
            #endif
            return result::STOP;

        case stack_error_t::SIZE_OVER_CAPACITY:
            printf("  SIZE: ");
            PRINT_COLOR(RED, "OVERFLOW\n");
            printf("    size = ");
            PRINT_COLOR_VAR(RED, "%zu\n", stack->size);
            printf("    capacity = ");
            PRINT_COLOR_VAR(RED, "%zu\n", stack->capacity);
            printf("\n");
            return result::STOP;

        case stack_error_t::ZERO_CAPACITY:
            printf("  CAPACITY: ");
            PRINT_COLOR(RED, "ZERO\n");
            return result::STOP;

        case stack_error_t::CAPACITY_TOO_LARGE:
            printf("  CAPACITY: ");
            PRINT_COLOR(RED, "EXCEEDS MAXIMUM LIMIT\n");
            printf("    capacity = ");
            PRINT_COLOR_VAR(RED, "%zu\n", stack->capacity);
            printf("    maximum allowed capacity = ");
            PRINT_COLOR_VAR(GREEN, "%zu\n", MAX_STACK_CAPACITY);
            printf("\n");
            return result::STOP;

        case stack_error_t::DATA_POINTER_CORRUPT:
            printf("  DATA_POINTER: ");
            PRINT_COLOR(RED, "CORRUPTED\n");
            printf("    data = ");
            PRINT_COLOR_VAR(RED, "%p", stack->data);
            if (stack->data == nullptr) {
                printf(" (null pointer)\n");
            } else {
                printf(" (suspicious value: %p)\n", stack->data);
            }
            return result::STOP;

        case stack_error_t::DATA_CANARY_CORRUPT:
            #ifdef DEBUG
            printf("  DATA_CANARY: ");
            PRINT_COLOR(RED, "CORRUPTED\n");

            printf("    left_data_canary = ");
            if (GetLeftDataCanary(stack) != CANARY_DEFAULT) {
                PRINT_STACK_CANARY(RED, GetLeftDataCanary(stack));
                printf(" (expected: ");
                PRINT_STACK_CANARY(GREEN, CANARY_DEFAULT);
                printf(")\n");
            } else {
                PRINT_STACK_CANARY(GREEN, GetLeftDataCanary(stack));
            }

            printf("    right_data_canary = ");
            if (GetRightDataCanary(stack) != CANARY_DEFAULT) {
                PRINT_STACK_CANARY(RED, GetRightDataCanary(stack));
                printf(" (expected: ");
                PRINT_STACK_CANARY(GREEN, CANARY_DEFAULT);
                printf(")\n");
            } else {
                PRINT_STACK_CANARY(GREEN, GetRightDataCanary(stack));
            }
            #endif
            break;

        case stack_error_t::HASH_MISMATCH:
            #ifdef DEBUG
            printf("  HASH: ");
            PRINT_COLOR(RED, "MISMATCH\n");
            printf("    struct_hash = ");
            PRINT_COLOR_VAR(RED, "%zu", stack->struct_hash);
            printf(" (current: ");
            PRINT_COLOR_VAR(GREEN, "%zu", CalculateStructHash(stack));
            printf(")\n");
            printf("    data_hash = ");
            PRINT_COLOR_VAR(RED, "%zu", stack->data_hash);
            printf(" (current: ");
            PRINT_COLOR_VAR(GREEN, "%zu", CalculateDataHash(stack));
            printf(")\n");
            #endif
            return result::STOP;

        case stack_error_t::POP_EMPTY_STACK:
            printf("  OPERATION: ");
            PRINT_COLOR(RED, "POP_FROM_EMPTY_STACK\n");
            break;
        case stack_error_t::OK:
        case stack_error_t::STACK_POINTER_CORRUPT:
        case stack_error_t::CALLOC_FAILED:
        case stack_error_t::REALLOC_FAILED:
            return result::STOP;
        default:
            return result::STOP;
    }
    return result::CONTINUE;
}

//----------------------------------------------------------------------------------

static result DumpStackData(stack_type* stack) {
    printf("  STACK_DATA (%zu/%zu):\n", stack->size, stack->capacity);

    #ifdef DEBUG
    PRINT_COLOR(BASE, "    [CANARY_LEFT] = ");
    stack_elem_t left_data_canary = GetLeftDataCanary(stack);
    if (left_data_canary == CANARY_DEFAULT) {
        PRINT_STACK_ELEMENT(GREEN, left_data_canary);
    } else {
        PRINT_STACK_ELEMENT(RED, left_data_canary);
    }
    #endif

    for (size_t pos = 0; pos < stack->capacity; pos++) {
        if (pos < stack->size) {
            PRINT_COLOR_VAR(BASE, "    *[%zu] = ", pos);
            PRINT_STACK_ELEMENT(GREEN, stack->data[pos]);
        } else if (pos == stack->size) {
            PRINT_COLOR_VAR(BASE, "    >[%zu] = ", pos);
            PRINT_STACK_ELEMENT(YELLOW, stack->data[pos]);
        } else {
            PRINT_COLOR_VAR(GREY, "    p[%zu] = ", pos);
            PRINT_STACK_ELEMENT(GREY, stack->data[pos]);
        }
    }

    #ifdef DEBUG
    PRINT_COLOR(BASE, "    [CANARY_RIGHT] = ");
    stack_elem_t right_data_canary = GetRightDataCanary(stack);
    if (right_data_canary == CANARY_DEFAULT) {
        PRINT_STACK_ELEMENT(GREEN, right_data_canary);
    } else {
        PRINT_STACK_ELEMENT(RED, right_data_canary);
    }
    #endif

    return result::STOP;
}

//----------------------------------------------------------------------------------
#ifdef DEBUG
static inline stack_elem_t GetLeftDataCanary(stack_type* stack) {
    return *(stack->data - 1);
}

//----------------------------------------------------------------------------------

static inline stack_elem_t GetRightDataCanary(stack_type* stack) {
    return stack->data[stack->capacity];
}
#endif
