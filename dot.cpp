#include "dot.h"

void StartDot(FILE* file_dot) {
    fprintf(file_dot,
        "digraph G{\n"
        "node[shape=Mrecord, style=filled];\n"
        "rankdir=TB;\n"
        "ordering=out;\n");
}

//----------------------------------------------------------------------------------

void MakeDotElement(FILE* file_dot, const size_t node, const char* params) {
    fprintf(file_dot, "node%zX [%s];\n", node, params);
}

//----------------------------------------------------------------------------------

void MakeDotElementConnection(FILE* file_dot, const size_t node1, const size_t node2, const char* section1, const char* section2, const char* params) {
    fprintf(file_dot, "node%zX%s -> node%zX%s [%s];\n", node1, section1, node2, section2, params);
}

//----------------------------------------------------------------------------------

void FinishDot(FILE* file_dot) {
    fprintf(file_dot, "}");
    fclose(file_dot);
}
