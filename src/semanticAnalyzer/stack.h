#pragma once
#include <deque>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <regex>
#include "tid.h"

using std::deque;
using std::find;

class type_stack {
public:
    type_stack();

    void push_stack(string lex, int d = 0);
    void push_stack(info i);
    void check_uno();
    void check_bin();
    bool check_if();
    info pop_stack();
private:
    deque<info> Operators_;
    deque<string> Operations_;
    const deque<string> simple_oper = { "+", "*", "/", "=", "-", "!", "<", ">", "&", "|", "%", "!" };
    const deque<string> compound_oper = { "==", ">=", "<=", "!=", "^=", "&=", "|=", "*=", "+=", "-=" };
};