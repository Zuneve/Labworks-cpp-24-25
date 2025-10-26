#include <algorithm>
#include <fstream>

#include "MyStructs.h"
#include "ParseTSV.h"

const size_t max_str_size = (1 << 8);

void Initalize2DListNode(Arguments* args) {
    if (args->rows > 0) {
        args->first_node->value = args->array[0][0];
        args->first_node->high = 0;
        args->first_node->len = 0;
    }

    Node2D* new_node;
    Node2D* first_in_col = args->first_node;
    Node2D* first_in_prev_col;

    for (int i = 0; i < args->rows; i++) {
        new_node = first_in_col;
        for (int j = 1; j < args->cols; j++) {
            PushRight(args->array[i][j], new_node);
            new_node = new_node->right;
            new_node->high = i;
            new_node->len = j;
            if (i > 0) {
                first_in_prev_col = first_in_prev_col->right;
                new_node->up = first_in_prev_col;
                first_in_prev_col->down = new_node;
            }
        }
        if (i < args->rows - 1) {
            PushDown(args->array[i + 1][0], first_in_col);
            first_in_prev_col = first_in_col;
            first_in_col = first_in_col->down;
        }
    }
    for (int i = 0; i < args->rows; i++) {
        delete[] args->array[i];
    }
    delete[] args->array;
}

void InitializeGridFromTSV(Arguments* args) {
    char* line = new char[max_str_size];
    int32_t mn_x = INT32_MAX;
    int32_t mn_y = INT32_MAX;
    int32_t mx_x = INT32_MIN;
    int32_t mx_y = INT32_MIN;
    while (fgets(line, max_str_size, args->input_file) != nullptr) {
        int32_t x, y;
        uint64_t sand;
        if (sscanf(line, "%d\t%d\t%lu", &x, &y, &sand) != 3) continue;
        mx_x = std::max(mx_x, x);
        mn_x = std::min(mn_x, x);
        mx_y = std::max(mx_y, y);
        mn_y = std::min(mn_y, y);
    }
    args->rows = mx_y - mn_y + 1;
    args->cols = mx_x - mn_x + 1;
    args->array = new uint64_t*[args->rows];
    args->mx_high = mx_y - mn_y;
    args->mx_len = mx_x - mn_x;
    args->mn_high = 0;
    args->mn_len = 0;
    for (int i = 0; i < args->rows; i++) {
        args->array[i] = new uint64_t[args->cols];
    }
    fseek(args->input_file, 0, SEEK_SET);
    while (fgets(line, max_str_size, args->input_file) != nullptr) {
        int32_t x, y;
        uint64_t sand;
        if (sscanf(line, "%d\t%d\t%lu", &x, &y, &sand) != 3) continue;
        x -= mn_x;
        y -= mn_y;
        args->array[y][x] = sand;
    }
    Initalize2DListNode(args);
    delete[] line;
}
