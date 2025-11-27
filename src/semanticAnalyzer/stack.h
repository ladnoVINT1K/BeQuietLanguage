#pragma once
#include <deque>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <regex>
#include "tid.h"

using std::deque;
using std::find;

struct infoStack {
    Types t_;
    int d_ = 0;
    string v_;

    infoStack(Types a) : t_(a) {}
    infoStack(Types a, int b) : t_(a), d_(b) {}
    infoStack(Types a, int b, string c) : t_(a), d_(b), v_(c) {}
};

class type_stack {
public:
    type_stack();

    void push_stack(string lex, int d = 0);
    void push_stack(info i);
    void push_stack(infoStack i);
    void check_uno();
    void check_bin();
    bool check_if();
    infoStack pop_stack();
private:
    deque<infoStack> Operators_;
    deque<string> Operations_;
    const deque<string> simple_oper = { "+", "*", "/", "=", "-", "!", "<", ">", "&", "|", "%", "!" };
    const deque<string> compound_oper = { "==", ">=", "<=", "!=", "^=", "&=", "|=", "*=", "+=", "-=" };
};