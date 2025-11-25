#pragma once
#include <unordered_map>
#include "types.h"
#include <string>
#include <deque>
#include <optional>
#include <stdexcept>
	

using std::string;
using std::unordered_map;
using std::deque;

struct info {
    Types t_;
    int d_ = 0;
    string v_ = "";

    info(Types a) : t_(a) {}
    info(Types a, int b) : t_(a), d_(b) {}
    info(Types a, int b, string c) : t_(a), d_(b), v_(c) {}
};

class tree_tid {
public:
    tree_tid();

    void push_id(string name, info i);
    std::optional<info> check_exist(string name);
    void create_tid();
    void delete_tid();
private:
    deque<unordered_map<string, info>> s_; 
};