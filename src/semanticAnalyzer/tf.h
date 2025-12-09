#pragma once
#include <optional>
#include <vector>
#include <string>

using std::string;
using std::vector;

enum class typefunc {
    Int,
    Float,
    Char,
    Void,
    Let
};

struct param {
    typefunc t_;
    int d_ = 0;
};

struct info_func {
    string name;
    string unic_name;
    param res;
    vector<param> params;
};

class tf {
public:
    tf();

    void new_func(info_func i);
    bool check_call(string name, vector<param> p);
    info_func call_res(string name, vector<param> p);

    vector<info_func> v_;
};