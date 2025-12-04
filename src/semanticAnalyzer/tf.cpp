#pragma once
#include "tf.h"
#include <exception>

tf::tf() {
    v_ = {};
}

bool tf::check_call(string name, vector<param> p) {
    for (auto i : v_) {
        if (i.name == name and p.size() == i.params.size()) {
            for (int j = 0; j < i.params.size(); ++j) {
                if ((i.params)[j].t_ != p[j].t_ or (i.params)[j].d_ != p[j].d_) return 0;
            }
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

info_func tf::call_res(string name, vector<param> p) {
    for (auto i : v_) {
        if (i.name == name) {
            for (int j = 0; j < i.params.size(); ++j) {
                if ((i.params)[j].t_ != p[j].t_ or (i.params)[j].d_ != p[j].d_) return {};
            }
            return i;
        }
    }
}

