#include <stdio.h>
#include <stdlib.h>
#include "stack_attack.h"

#ifdef DEBUG

// Повреждение канареек структуры
void AttackCorruptStructCanaries(stack_type* stack) {
    printf("[ATTACK] Corrupting struct canaries...\n");

    // Повреждаем обе канарейки структуры
    stack->left_canary  = 0xDEADBEEF;
    stack->right_canary = 0xBADF00D;
}

// Повреждение хешей
void AttackCorruptHashes(stack_type* stack) {
    printf("[ATTACK] Corrupting hashes...\n");

    // Портим оба хеша
    stack->struct_hash = 0x12345678;
    stack->data_hash   = 0x87654321;

    // Слегка меняем данные чтобы хеши точно не совпали
    if (stack->size > 0) {
        stack->data[0] = 0x666;
    }
}

// Создание неконсистентности размера
void AttackCreateSizeInconsistency1(stack_type* stack) {
    printf("[ATTACK] Creating size inconsistency...\n");

    // Устанавливаем недопустимые размеры
    stack->size     = -1;
    stack->capacity = -1;
}

void AttackCreateSizeInconsistency2(stack_type* stack) {
    printf("[ATTACK] Creating size inconsistency...\n");

    // Устанавливаем размер больше емкости
    stack->size = stack->capacity + 1;
}

// Повреждение канареек данных
void AttackCorruptDataCanaries(stack_type* stack) {
    printf("[ATTACK] Corrupting data canaries...\n");

    // Портим левую канарейку данных
    stack_elem_t* left_canary_ptr = stack->data - 1;
    *left_canary_ptr = 0xBADF00D;

    // Портим правую канарейку данных
    stack_elem_t* right_canary_ptr = stack->data + stack->capacity;
    *right_canary_ptr = 0xDEADC0DE;
}

// Комбинированная атака
void AttackCombinedDestruction(stack_type* stack) {
    printf("[ATTACK] Launching combined destruction...\n");

    // 1. Портим канарейки структуры
    stack->left_canary  = 0;
    stack->right_canary = 0;

    // 2. Портим хеши
    stack->struct_hash = 0;
    stack->data_hash   = 0;

    // 3. Создаем неконсистентность размера
    stack->size = stack->capacity + 50;

    // 4. Портим канарейки данных
    if (stack->data != nullptr) {
        stack_elem_t* left_canary = stack->data - 1;
        *left_canary = 0;

        stack_elem_t* right_canary = stack->data + stack->capacity;
        *right_canary = 0;
    }
}

// Утилита для "тихой смерти"
void AttackSilentDeath(stack_type* stack) {
    printf("[ATTACK] Preparing silent death...\n");

    // Делаем указатель на данные подозрительным
    stack->data = (stack_elem_t*)0x1;
}

#endif
