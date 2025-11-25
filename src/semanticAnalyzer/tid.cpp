#pragma once
#include "tid.h"

tree_tid::tree_tid() {
    s_.push_front({});
    // s_.push_front({ {"inter", {Types::Num, 0, "12"}} });
}

void tree_tid::push_id(string name, info i) {
    auto ptr = &s_.front();
    if (check_exist(name)) return;
    ptr->insert({name, i});
    return;
}

std::optional<Types> tree_tid::check_exist(string name) {
    for (auto i : s_) {
        // i - unordered_map
        if (i.find(name) != i.end()) return ((i.find(name))->second).t_;
    }
    return {};
}

void tree_tid::create_tid() {
    s_.push_front({});
    return;
}

void tree_tid::delete_tid() {
    if (s_.size() == 1) 
        throw std::runtime_error("don't delete root TID");
    else {
        s_.pop_front();
    }
    return;
}