#ifndef _DEFENDER_SYSTEM_H_
#define _DEFENDER_SYSTEM_H_

#include "define_lib.h"
#include "stack_lib.h"

#ifdef DEBUG
const size_t HASH_SEED = 5381;
#endif

typedef struct VerifyCallData {
    const char* file_name = nullptr;
    int line_number       = 0;
    const char* func_name = nullptr;
} call_data_t;

typedef const enum class DumpResult: char {
    CONTINUE = 0,
    STOP = 1
} result;

#define CHECK_ERROR(error) if (error != stack_error_t::OK) { return (int)error; }

#define GET_INFO(name) call_data_t name = {__FILE__, __LINE__, __PRETTY_FUNCTION__}

#ifdef DEBUG
#define STACK_VERIFY_AND_RETURN(st, str) BEGIN { \
                                        GET_INFO(call_info); \
                                        stack_error_t err = CheckStackIntegrity(st); \
                                        if (err != stack_error_t::OK) { \
                                            if (err != stack_error_t::STACK_POINTER_CORRUPT) { \
                                                st->error = err; \
                                            } \
                                            StackDump(st, &call_info, str); \
                                            return err; \
                                        } \
                                    } END

#define CHECK_CAPACITY_AND_RETURN(stack, capacity) BEGIN {  if (capacity > MAX_STACK_CAPACITY) { \
                                                                GET_INFO(call_info); \
                                                                stack->error = stack_error_t::CAPACITY_TOO_LARGE; \
                                                                StackDump(stack, &call_info, "requested capacity exceeds maximum limit"); \
                                                                return stack_error_t::CAPACITY_TOO_LARGE; \
                                                            } \
                                                         } END
#else
#define STACK_VERIFY_AND_RETURN(st, str) BEGIN { \
                                        stack_error_t err = CheckStackIntegrity(st); \
                                        if (err != stack_error_t::OK) { \
                                            if (err != stack_error_t::STACK_POINTER_CORRUPT) { \
                                                st->error = err; \
                                            } \
                                            StackDump(st, str); \
                                            return err; \
                                        } \
                                    } END

#define CHECK_CAPACITY_AND_RETURN(stack, capacity) BEGIN {  if (capacity > MAX_STACK_CAPACITY) { \
                                                                stack->error = stack_error_t::CAPACITY_TOO_LARGE; \
                                                                StackDump(stack, "requested capacity exceeds maximum limit"); \
                                                                return stack_error_t::CAPACITY_TOO_LARGE; \
                                                            } \
                                                         } END
#endif


stack_error_t CheckStackIntegrity(stack_type* stack);
#ifdef DEBUG
void StackDump(stack_type* stack, call_data_t* call_info, const char* message);
size_t CalculateStructHash(stack_type* stack);
size_t CalculateDataHash(stack_type* stack);
#else
void StackDump(stack_type* stack, const char* message);
#endif

#endif // _DEFENDER_SYSTEM_H_
