#include <algorithm>

#include <lib/GenBMP.h>
#include <lib/ParseArguments.h>
#include <lib/ParseTSV.h>
#include <lib/MyStructs.h>

void AddBlackPixels(Arguments* args) {
    Node2D* new_node;
    Node2D* first_in_col = args->first_node;

    for (int i = args->mn_high; i <= args->mx_high; i++) {
        new_node = first_in_col;
        for (int j = args->mn_len; j <= args->mx_len; j++) {
            if (new_node->value > 3) {
                args->black_pixels.push_back(new_node);
            }
            new_node = new_node->right;
        }
        first_in_col = first_in_col->down;
    }
}

void InsertRowUp(Arguments* args, Node2D* black_node, uint64_t cnt) {
    PushUp(cnt, black_node);
    if (args->mn_high > black_node->up->high) {
        Node2D* cur_node = args->first_node;
        args->mn_high = black_node->up->high;

        for (int32_t i = args->mn_len; i <= args->mx_len; i++) {
            if (!cur_node->up) {
                PushUp(0, cur_node);
            }
            if (i == args->mn_len) {
                cur_node = cur_node->right;
                continue;
            }
            (cur_node->up)->left = cur_node->left->up;
            (cur_node->left->up)->right = cur_node->up;
            cur_node = cur_node->right;
        }

        args->first_node = args->first_node->up;
        delete cur_node;
    }
}

void InsertRowDown(Arguments* args, Node2D* black_node, uint64_t cnt) {
    PushDown(cnt, black_node);
    if (args->mx_high < black_node->down->high) {
        args->mx_high = black_node->down->high;
        Node2D* cur_node = black_node;
        while (cur_node->left) {
            cur_node = cur_node->left;
        }

        for (int32_t i = args->mn_len; i <= args->mx_len; i++) {
            if (!cur_node->down) {
                PushDown(0, cur_node);
            }
            if (i == args->mn_len) {
                cur_node = cur_node->right;
                continue;
            }
            (cur_node->down)->left = cur_node->left->down;
            (cur_node->left->down)->right = cur_node->down;
            cur_node = cur_node->right;
        }

        delete cur_node;
    }
}

void InsertColonLeft(Arguments* args, Node2D* black_node, uint64_t cnt) {
    PushLeft(cnt, black_node);
    if (args->mn_len > black_node->left->len) {
        args->mn_len = black_node->left->len;
        Node2D* cur_node = args->first_node;

        for (int32_t i = args->mn_high; i <= args->mx_high; i++) {
            if (!cur_node->left) {
                PushLeft(0, cur_node);
            }
            if (i == args->mn_high) {
                cur_node = cur_node->down;
                continue;
            }
            (cur_node->up->left)->down = cur_node->left;
            (cur_node->left)->up = cur_node->up->left;
            cur_node = cur_node->down;
        }

        args->first_node = args->first_node->left;
        delete cur_node;
    }
}

void InsertColonRight(Arguments* args, Node2D* black_node, uint64_t cnt) {
    PushRight(cnt, black_node);
    if (args->mx_len < black_node->right->len) {
        args->mx_len = black_node->right->len;
        Node2D* cur_node = black_node;
        while (cur_node->up) {
            cur_node = cur_node->up;
        }

        for (int32_t i = args->mn_high; i <= args->mx_high; i++) {
            if (!cur_node->right) {
                PushRight(0, cur_node);
            }
            if (i == args->mn_high) {
                cur_node = cur_node->down;
                continue;
            }
            (cur_node->right)->up = cur_node->up->right;
            (cur_node->up->right)->down = cur_node->right;
            cur_node = cur_node->down;
        }

        delete cur_node;
    }
}

void NextIteration(Arguments* args) {
    while (!args->black_pixels.empty()) {
        Node2D* black_node = args->black_pixels.pop_back();
        uint64_t cnt = black_node->value / 4;
        black_node->value %= 4;

        if (black_node->up) {
            black_node->up->value += cnt;
        } else {
            InsertRowUp(args, black_node, cnt);
        }

        if (black_node->down) {
            black_node->down->value += cnt;
        } else {
            InsertRowDown(args, black_node, cnt);
        }

        if (black_node->left) {
            black_node->left->value += cnt;
        } else {
            InsertColonLeft(args, black_node, cnt);
        }

        if (black_node->right) {
            black_node->right->value += cnt;
        } else {
            InsertColonRight(args, black_node, cnt);
        }
    }
    AddBlackPixels(args);
}

void SandPileIterations(Arguments* args) {
    AddBlackPixels(args);
    int image_iteration = 0;
    for (int i = 0; i < args->max_iter; i++) {
        NextIteration(args);
        if (args->freq != 0 && i % args->freq == args->freq - 1) {
            CreateBMPImage(args, image_iteration++);
            if (args->black_pixels.empty()) {
                return;
            }
        }
        if (args->black_pixels.empty()) {
            break;
        }
    }
    args->black_pixels.clear();
    CreateBMPImage(args, image_iteration);
    fclose(args->input_file);
}

void ClearMatrixPointers(Node2D* node) {
    while (node != nullptr) {
        Node2D* next_right_node = node->right;
        Node2D* current = node;

        while (current != nullptr) {
            Node2D* next_down_node = current->down;
            delete current;
            current = next_down_node;
        }

        node = next_right_node;
    }
}

int main(int argc, char** argv) {
    Arguments* args = new Arguments;
    args->argc = argc;
    args->argv = argv;

    SetArguments(args);
    InitializeGridFromTSV(args);
    SandPileIterations(args);

    if (args->first_node) {
        ClearMatrixPointers(args->first_node);
    }
    delete args;
    return 0;
}
