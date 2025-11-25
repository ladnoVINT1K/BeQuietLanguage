#pragma once
#include <unordered_map>
#include <vector>
#include <string>
#include "types.h"

using std::string;
using std::vector;

enum class typefunc {
    Int,
    Float,
    String,
    Void,
    Let
};

struct param {
    typefunc t_;
    int d_ = 0;
};

struct info_func {
    string name;
    param res;
    vector<param> params;
};

class tf {
public:
    tf();

    void new_func(info_func i);
    bool check_call(string name, vector<param> p);
    Types stack_type(string name);
private:
    std::unordered_map<string, info_func> table_;
};