#pragma once
#include "trie.h"

void Trie::addString(string &s) {
    auto v = root;
    for (auto c : s) {
        int i = c - 'a';
        if (!v->edges[i]) {
            v->edges[i] = new vertex();
        }
        v = v->edges[i];
    }
    v->isTerm = true;
}

bool Trie::isExisted(string &s) {
    auto v = root;
    for (auto c : s) {
        int i = c - 'a';
        if (!v->edges[i]) return false;
        v = v->edges[i];
    }
    return v->isTerm;
}

void loadFromfile(std::ifstream& in, Trie B) {
    string tmp;
    while (!in.eof()) {
        in >> tmp;
        B.addString(tmp);
    }
    return;
}
