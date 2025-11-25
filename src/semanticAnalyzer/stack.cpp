#pragma once
#include "stack.h"

type_stack::type_stack() {}

void type_stack::push_stack(string lex, int d = 0) {
    if (find(simple_oper.begin(), simple_oper.end(), lex) != simple_oper.end() or
        find(compound_oper.begin(), compound_oper.end(), lex) != compound_oper.end()) {
        Operations_.push_front(lex);
    } else {
        Types t;
        string v = lex;
        std::regex n("^\d+(\.\d+)?$");
        if (std::regex_match(lex, n)) {
            t = Types::Num;
            Operators_.push_front({t, d, v});
            return;
        } else {
            if (lex == "true" or lex == "false") {
                t = Types::Bool;
                Operators_.push_front({t, d, v});
                return;
            } else {
                t = Types::Str;
                Operators_.push_front({t, d, v});
                return;
            }
        }
    }
}

void type_stack::push_stack(info i) {
    Operators_.push_front(i);
    return;
}

void type_stack::check_uno() {
    info c = Operators_.front(); Operators_.pop_front();
    string op = Operations_.front(); Operations_.pop_front();

    if (c.d_ != 0) throw std::runtime_error("can't use uno oper for massive");

    if (op == "-" and c.t_ == Types::Num) {
        push_stack(c);
        return;
    } else if (op == "!" and c.t_ == Types::Bool) {
        push_stack(c);
        return;
    } else {
        throw std::runtime_error("error in uno operation");
    }
}

void type_stack::check_bin() {
    info c1 = Operators_.front(); Operators_.pop_front();
    info c2 = Operators_.front(); Operators_.pop_front();
    string op = Operations_.front(); Operations_.pop_front();

    if (op == "=") {
        if (c1.d_ == c2.d_ and c1.t_ == c2.t_) c2 = c1;
        else throw std::runtime_error("error in assignment");
        push_stack(c2);
        return; 
    }

    if (c1.d_ != 0 or c2.d_ != 0 or c1.t_ != c2.t_) throw std::runtime_error("can't use bin oper for massive");
    else {
        if (c1.t_ == Types::Bool && (op == "==" || op == "!=" || op == "and" || op == "or")) {
            info res(Types::Bool, 0);
            push_stack(res);
            return;
        } else if (c1.t_ == Types::Str && (op == "+" || op == "==" || op == "!=")) {
            info res(Types::Str);
            push_stack(res);
            return;
        } else if (c1.t_ == Types::Num && 
            (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" ||
            op == "==" || op == "!=" || op == "<" || op == ">" ||
            op == "<=" || op == ">=" || op == "+=" || op == "-=" ||
            op == "*=" || op == "/=" || op == "%=")) {
            info res(Types::Num, 0);
            push_stack(res);
            return;
        } else {
            throw std::runtime_error("error in check_bin!");
        }
    }
}

bool type_stack::check_if() {
    info c = Operators_.front(); Operations_.pop_front();
    if (c.t_ == Types::Bool or c.t_ == Types::Bool and c.d_ == 0) return true;
    return false;
}

info type_stack::pop_stack() {
    info c = Operators_.front(); Operations_.pop_front();
    return c;
}