#ifndef _STACK_FUNC_H_
#define _STACK_FUNC_H_

#include "color_lib.h"
#include "stack_lib.h"

#include "defender_system.h"

#ifdef DEBUG
#define INIT(name) .stack_info = {__FILE__, __LINE__, __PRETTY_FUNCTION__, #name}
#else
#define INIT(name)
#endif

#define MAKE_STACK(name) stack_type name = {INIT(name)}; StackCtor(&name, 5)


stack_error_t StackCtor(stack_type* stack, const size_t capacity);
stack_error_t StackPush(stack_type* stack, const stack_elem_t value);
stack_error_t StackPop(stack_type* stack, stack_elem_t* value);
stack_error_t StackDtor(stack_type* stack);
stack_error_t PrintStack(stack_type* stack);

#endif // _STACK_FUNC_H_
