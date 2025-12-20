#pragma once
#include <unordered_map>
#include <string>
#include <deque>
#include <optional>
#include <stdexcept>

enum class TypesId {
    Int,
    Float,
    Char
};

using std::string;
using std::unordered_map;
using std::deque;
using std::optional;

struct info {
    TypesId t_;
    int d_ = 0;
    string v_ = "";

    info(TypesId a) : t_(a) {}
    info(TypesId a, int b) : t_(a), d_(b) {}
    info(TypesId a, int b, string c) : t_(a), d_(b), v_(c) {}
};

class tree_tid {
public:
    tree_tid();

    void push_id(string name, info i);
    optional<info> check_exist_init(string name);
    optional<info> check_exist_expr(string name);
    void change_val(string name, string val);
    void create_tid();
    void delete_tid();
private:
    deque<unordered_map<string, info>> s_;
};