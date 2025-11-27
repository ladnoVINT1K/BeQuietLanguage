#pragma once
#include "tf.h"
#include <exception>

tf::tf() {
    v_ = {};
}

bool tf::check_call(string name, vector<param> p) {
    for (auto i : v_) {
        if (i.name == name and i.params == p) {
            return 1;
        }
    }
    return 0;
}

void tf::new_func(info_func i) {
    if (check_call(i.name, i.params)) return;
    else {
        v_.push_back(i);
        return;
    }
}

