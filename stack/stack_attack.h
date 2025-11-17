#ifndef _STACK_ATTACK_H_
#define _STACK_ATTACK_H_

#include "stack_lib.h"
#include "stack_func.h"

#ifdef DEBUG

void AttackCorruptStructCanaries(stack_type* stack);
void AttackCorruptHashes(stack_type* stack);
void AttackCreateSizeInconsistency1(stack_type* stack);
void AttackCreateSizeInconsistency2(stack_type* stack);
void AttackCorruptDataCanaries(stack_type* stack);
void AttackCombinedDestruction(stack_type* stack);
void AttackSilentDeath(stack_type* stack);

#endif

#endif // _STACK_ATTACK_H_
