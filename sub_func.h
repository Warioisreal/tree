#ifndef _SUB_FUNC_H_
#define _SUB_FUNC_H_


#include <stdio.h>

#include "tree.h"


const int ERROR_COMP_ANSW_RES = -10;


void CleanInput(FILE* file);
void SkipSpaces(char* buffer, int* position);
int CompareAnswer(const char* answer);

void MakeGreenElem(Node_t* node);
void MakeYellowElem(Node_t* node);
void MakeRedElem(Node_t* node);
void MakeGreyElem(Node_t* node);

size_t CalculateNodeHash(Node_t* node);

#endif //_SUB_FUNC_H_
