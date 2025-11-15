#pragma once
#include "trie.h"

using std::isalpha;
using std::isalnum;
using std::isdigit;
using std::find;

const enum class Types { Keyword, Identificator, Literal, Operation, Punctuation, ELSE, END };

string type_to_string(Types type);

struct Lexem {
    Types type;
    std::string value;
    int line;
    int column;

    Lexem(Types t, const std::string& v, int ln, int col) : type(t), value(v), line(ln), column(col) {}
};

class Lexer {
public:
    Lexer(string file, Trie& trie);
    ~Lexer();
    Lexem get_lexem();
private:
    char* text_;
    char* pos_, * end_;
    int current_line_ = 1;
    int current_column_ = 1;
    Trie& trie_;
    const vector <char> simple_oper = { '+', '*', '/', '=', '-', '!', '<', '>', '&', '|', '%', '^', '[', ']', '.', '!' };
    const vector <string> compound_oper = { "==", ">=", "<=", "!=", "^=", "&=", "|=", "*=", "+=", "-=" };
    Lexem make_lexem(Types type, const std::string& value, int line, int col);
};
