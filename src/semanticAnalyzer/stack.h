#pragma once
#include <deque>
#include <string>
#include <stdexcept>
#include <algorithm>
#include "../lexicalAnalyzer/lexer.cxx"
#include <regex>
#include "tid.h"
#include "tf.h"

using std::deque;
using std::find;

enum class typestack {
    Int,
    Float,
    Void,
    Char,
    Bool
};

struct infoStack {
    typestack t_;
    Types l_;
    int d_ = 0;
    string v_;

    infoStack(typestack a) : t_(a) {}
    infoStack(typestack a, Types d) : t_(a), l_(d) {}
    infoStack(typestack a, Types d, int b) : t_(a), l_(d), d_(b) {}
    infoStack(typestack a, Types d, int b, string c) : t_(a), l_(d), d_(b), v_(c) {}
};

class type_stack {
public:
    type_stack();

    void push_stack(string lex, int d = 0);
    void push_stack(info i);
    void push_stack(infoStack i);
    void push_stack(info_func i);
    void check_uno();
    void check_bin();
    bool check_if();
    infoStack pop_stack();
    typestack to_stackt(TypesId type);
private:
    deque<infoStack> Operators_;
    deque<string> Operations_;
    const deque<string> simple_oper = { "+", "*", "/", "=", "-", "!", "<", ">", "&", "|", "%", "!" };
    const deque<string> compound_oper = { "==", ">=", "<=", "!=", "^=", "&=", "|=", "*=", "+=", "-=" };
};