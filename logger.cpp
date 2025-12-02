#include <stdio.h>
#include <stdlib.h>

#include "dot.h"
#include "logger.h"

static void GoLogRec(Node_t* node, FILE* file_dot);

void GetFullFolderName(const char* list_name, char* buffer) {
    const char* folder_name = LOG_FOLDER;
    snprintf(buffer, LOG_FOLDER_NAME_SIZE, "%s_%s", folder_name, list_name);
}

//----------------------------------------------------------------------------------

void UpdateFolder(const char* name_folder) {
    char command[COMMAND_SIZE] = "";
    snprintf(command, LOG_FOLDER_NAME_SIZE, "%s%s", "rm -r ", name_folder);
    system(command);
    snprintf(command, LOG_FOLDER_NAME_SIZE, "%s%s", "mkdir ", name_folder);
    system(command);
}

//----------------------------------------------------------------------------------

void StartLog(FILE** file, const char* list_name) {
    char filename[LOG_FILE_NAME_SIZE] = "";
    snprintf(filename, LOG_FOLDER_NAME_SIZE, "%s_%s.htm", DUMP_FILENAME, list_name);
    *file = fopen(filename, "wb");
}

//----------------------------------------------------------------------------------

void FinishLog(FILE** file) {
    fclose(*file);
    *file = nullptr;
}

//----------------------------------------------------------------------------------

void GoLog(Node_t* root, size_t size, Node_t* node, const char* message, LOG* log) {
    char filename_dot[DOT_FILE_NAME_SIZE] = "";
    snprintf(filename_dot, DOT_FILE_NAME_SIZE, "%s_%s/dot_%s_%zu.txt", LOG_FOLDER, log->name, log->name, log->dump_count);
    FILE* file_dot = fopen(filename_dot, "wb");
    StartDot(file_dot);

    GoLogRec(node, file_dot);

    FinishDot(file_dot);

    char command[COMMAND_SIZE] = "";
    snprintf(command, COMMAND_SIZE, "dot -Tsvg %s_%s/dot_%s_%zu.txt -o %s_%s/dot_%s_%zu.svg", LOG_FOLDER, log->name, log->name, log->dump_count, LOG_FOLDER, log->name, log->name, log->dump_count);
    system(command);

    fprintf(log->file_log,
        "<div style=\"margin: 20px; padding: 15px; border: 2px solid #4CAF50; border-radius: 10px; background: #f8f9fa; box-shadow: 0 4px 8px rgba(0,0,0,0.1);\">\n"
            "<h2 style=\"color: #2E86AB; margin-top: 0;\">Log \"%s\" Debug %zu</h2>\n"
            "<h2 style=\"margin-top: 0;\">%s</h2>\n"
            "<div style=\"width: 20%%;\">\n"
                "<h3 style=\"color: #A23B72;\">Parameters</h3>\n"
                "<table style=\"width: 100%%; border-collapse: collapse;\">\n"
                    "<tr style=\"background: #e9ecef;\">"
                        "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">ROOT</td>"
                        "<td style=\"padding: 8px; border: 1px solid #ddd;\">%zX</td>"
                    "</tr>\n"
                    "<tr>"
                        "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">SIZE</td>"
                        "<td style=\"padding: 8px; border: 1px solid #ddd;\">%zu</td>"
                    "</tr>\n"
                    "<tr style=\"background: #e9ecef;\">"
                        "<td style=\"padding: 8px; border: 1px solid #ddd; font-weight: bold;\">LOGNAME</td>"
                        "<td style=\"padding: 8px; border: 1px solid #ddd;\">%s</td>"
                    "</tr>\n"
                "</table>\n"
            "</div>\n"
            "<div style=\"min-width: 40%%;\">\n"
                "<h3 style=\"color: #A23B72;\">Visualization</h3>\n"
                "<div style=\"overflow-x: auto;\">\n"
                    "<img src=\"%s_%s/dot_%s_%zu.svg\" style=\"max-width: none; height: auto; border: 1px solid #ddd; border-radius: 5px;\" alt=\"List visualization\">\n"
                "</div>\n"
            "</div>\n"
            "<hr style=\"margin: 15px 0; border: 0; border-top: 1px solid #ccc;\">\n"
            "<small style=\"color: #666;\">Generated from: %s</small>\n"
        "</div>\n\n",
        log->name,
        log->dump_count,
        message,
        (size_t)root,
        size,
        log->name,
        LOG_FOLDER,
        log->name,
        log->name,
        log->dump_count++,
        filename_dot
    );
}

//----------------------------------------------------------------------------------

static void GoLogRec(Node_t* node, FILE* file_dot) {
    char params[DOT_PARAMS_SIZE] = "";
    char left[CHILD_TEXT_PTR_SIZE] = "";
    char right[CHILD_TEXT_PTR_SIZE] = "";
    if (node->left != nullptr) {
        snprintf(left, CHILD_TEXT_PTR_SIZE, "Да");
    } else {
        snprintf(left, CHILD_TEXT_PTR_SIZE, "0");
    }
    if (node->right != nullptr) {
        snprintf(right, CHILD_TEXT_PTR_SIZE, "Нет");
    } else {
        snprintf(right, CHILD_TEXT_PTR_SIZE, "0");
    }
    snprintf(
        params,
        DOT_PARAMS_SIZE,
        "color=\"#%06zX\", fillcolor=\"#%06zX\", penwidth=\"2\", label=\"{addr = %zX | value = %s | {<left> %s | <right> %s}}\"",
        node->color,
        node->bg_color,
        (size_t)node,
        node->value,
        left,
        right);

    MakeDotElement(file_dot, (size_t)node, params);
    if (node->left != nullptr) {
        MakeDotElementConnection(file_dot, (size_t)node, (size_t)node->left, ": <left>", "", "color=\"#ff0000\"");
        GoLogRec(node->left, file_dot);
    }
    if (node->right != nullptr) {
        MakeDotElementConnection(file_dot, (size_t)node, (size_t)node->right, ": <right>", "", "color=\"#0000ff\"");
        GoLogRec(node->right, file_dot);
    }
}
