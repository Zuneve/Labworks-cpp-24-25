#include "MyStructs.h"

void PushLeft(uint64_t val, Node2D* cur_node) {
    Node2D* new_node = new Node2D;
    new_node->value = val;
    cur_node->left = new_node;
    new_node->right = cur_node;
    new_node->len = cur_node->len - 1;
}

void PushRight(uint64_t val, Node2D* cur_node) {
    Node2D* new_node = new Node2D;
    new_node->value = val;
    cur_node->right = new_node;
    new_node->left = cur_node;
    new_node->len = cur_node->len + 1;
}

void PushUp(uint64_t val, Node2D* cur_node) {
    Node2D* new_node = new Node2D;
    new_node->value = val;
    cur_node->up = new_node;
    new_node->down = cur_node;
    new_node->high = cur_node->high - 1;
}

void PushDown(uint64_t val, Node2D* cur_node) {
    Node2D* new_node = new Node2D;
    new_node->value = val;
    cur_node->down = new_node;
    new_node->up = cur_node;
    new_node->high = cur_node->high + 1;
}
