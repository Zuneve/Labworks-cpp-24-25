#pragma once

#include <fstream>
#include <iostream>

template<typename T>
struct Node {
    T value;
    Node* next = nullptr;
    Node* prev = nullptr;
};

template<typename T>
struct ListNode {
    Node<T>* tail = nullptr;
    Node<T>* head = nullptr;

    void push_back(T val) {
        Node<T>* cur_node = new Node<T>;
        cur_node->value = val;
        if (!tail) {
            head = cur_node;
            tail = cur_node;
        } else {
            tail->next = cur_node;
            cur_node->prev = tail;
            tail = cur_node;
        }
    }

    void push_front(T val) {
        Node<T>* cur_node = new Node<T>;
        cur_node->value = val;
        if (!head) {
            head = cur_node;
            tail = cur_node;
        } else {
            cur_node->next = head;
            head->prev = cur_node;
            head = cur_node;
        }
    }

    T pop_back() {
        if (tail == nullptr) {
            std::cerr << "Error: Cannot pop from back: ListNode is empty" << std::endl;
            return T();
        }
        Node<T>* prev_node = tail->prev;
        T val = tail->value;
        delete tail;
        tail = prev_node;
        if (tail) {
            tail->next = nullptr;
        } else {
            head = nullptr;
        }
        return val;
    }

    T top() {
        if (tail == nullptr) {
            std::cerr << "Error: Cannot get top: ListNode is empty" << std::endl;
        }
        return tail->value;
    }

    void pop_front() {
        if (tail == nullptr) {
            std::cerr << "Error: Cannot pop from front: ListNode is empty" << std::endl;
            return;
        }
        Node<T>* next_node = head->next;
        delete head;
        head = next_node;
        if (head) {
            head->prev = nullptr;
        } else {
            tail = nullptr;
        }
    }

    bool empty() {
        return tail == nullptr;
    }

    void clear() {
        Node<T>* current = head;
        while (current) {
            Node<T>* nextNode = current->next;
            delete current;
            current = nextNode;
        }
        head = nullptr;
    }

    ~ListNode() {
        clear();
    }
};

struct Node2D {
    uint64_t value;
    int32_t high;
    int32_t len;
    Node2D* up = nullptr;
    Node2D* down = nullptr;
    Node2D* left = nullptr;
    Node2D* right = nullptr;
};

struct Arguments {
    int32_t argc;
    char** argv;
    FILE* input_file;
    char* output_file;
    ListNode<Node2D*> black_pixels;
    uint64_t** array;
    int32_t rows;
    int32_t cols;
    int32_t max_iter = INT32_MAX;
    int32_t freq = 0;
    Node2D* first_node = new Node2D;
    int32_t mx_high = 0;
    int32_t mn_high = 0;
    int32_t mx_len = 0;
    int32_t mn_len = 0;
};

void PushLeft(uint64_t val, Node2D* cur_node);

void PushRight(uint64_t val, Node2D* cur_node);

void PushUp(uint64_t val, Node2D* cur_node);

void PushDown(uint64_t val, Node2D* cur_node);
