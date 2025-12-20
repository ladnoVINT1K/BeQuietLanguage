#include "lexer.h"

Lexer::Lexer(string file, Trie& trie) : trie_(trie) {
    std::ifstream in_program("Program.txt", std::ios::binary);
    in_program.seekg(0, std::ios::end);
    int size_text = in_program.tellg();
    in_program.seekg(0, std::ios::beg);
    text_ = new char[size_text];
    in_program.read(text_, size_text);
    pos_ = text_;
    end_ = text_ + size_text;
}

Lexer::~Lexer() {}

Lexem Lexer::make_lexem(Types type, const std::string& value, int line, int col) {
    return Lexem(type, value, line, col);
}

Lexem Lexer::get_lexem() {
    int start_line = current_line_;
    int start_col = current_column_;
    Types type = Types::Keyword;
    for (; pos_ < end_;) {
        string res = "";
        if (*pos_ == ' ' or *pos_ == '\n' or *pos_ == '\r' or *pos_ == '\t') {
            if (*pos_ == '\n') {
                ++current_line_;
                ++start_line;
                current_column_ = 0;
                start_col = 0;
            }
            ++pos_;
            ++current_column_;
            ++start_col;
            continue;
        } else if (*pos_ == '/') {
            ++pos_;
            ++current_column_;
            if (pos_ < end_ and *pos_ == '=') {
                type = Types::Operation;
                res += *(pos_++);
            } else if (pos_ < end_ and *pos_ == '*') {
                ++pos_;
                ++current_column_;
                while (pos_ < end_) {
                    if (*(pos_++) == '*' and *pos_ == '/') {
                        current_column_ += 2;
                        ++pos_;
                        break;
                    }
                }
                continue;
            } else if (pos_ < end_ and *pos_ == '/') {
                ++pos_;
                ++current_column_;
                while (pos_ < end_ and *(pos_++) != '\n') {}
                continue;
            } else {
                type = Types::Operation;
                res += '/';
            }
        } else if (isalpha(*pos_) or *pos_ == '_') {
            res += (*(pos_++));
            ++current_column_;
            bool num = false;;
            while (pos_ < end_ and (isalnum(*pos_) or *pos_ == '_')) {
                res += (*(pos_++));
                if ((*pos_ >= '0' and *pos_ <= '9') or *pos_ == '_') num = true;
                ++current_column_;
            }
            if (num) type = Types::Identificator;
            else if (res == "not" or res == "and" or res == "or") {
                type = Types::Operation;
            } else if (res == "true" or res == "false") {
                type = Types::Literal;
            } else if (trie_.isExisted(res)) {
                type = Types::Keyword;
            } else type = Types::Identificator;
        } else if (isdigit(*pos_)) {
            while (pos_ < end_ and isdigit(*pos_)) {
                res += (*(pos_++));
                ++current_column_;
            }
            if (pos_ < end_ and *pos_ == '.') {
                ++pos_;
                ++current_column_;
                res += '.';
                while (pos_ < end_ and isdigit(*pos_)) {
                    res += (*(pos_++));
                    ++current_column_;
                }
            }
            if (res[res.size() - 1] == '.') {
                type = Types::ELSE;
            } else type = Types::Literal;
        } else if (*pos_ == ',' or *pos_ == ';' or *pos_ == '(' or *pos_ == ')' or *pos_ == '{' or *pos_ == '}') {
            res += *pos_;
            type = Types::Punctuation;
            ++pos_;
            ++current_column_;
        } else if (*pos_ == '\0') {
            type = Types::Literal;
            res += *(pos_++);
            ++current_column_;
        } else if (*pos_ == '"') {
            res += *(pos_++);
            ++current_column_;
            while (pos_ < end_ and *pos_ != '"') {
                res += *(pos_++);
                ++current_column_;
            }
            if (pos_ < end_) {
                res += *(pos_++);
                ++current_column_;
                if (res.size() > 3) type = Types::ELSE;
                else type = Types::Literal;
            } else {
                type = Types::ELSE;
            }
        } else if (*pos_ == '\'') {
            res += *(pos_++);
            ++current_column_;
            while (pos_ < end_ and *pos_ != '\'') {
                res += *(pos_++);
            }
            if (pos_ < end_) {
                res += *(pos_++);
                ++current_column_;
                if (res.size() > 3) type = Types::ELSE;
                else type = Types::Literal;
            } else {
                type = Types::ELSE;
            }
        } else if (find(simple_oper.begin(), simple_oper.end(), *pos_) != simple_oper.end()) {
            res += *pos_;
            type = Types::Operation;
            ++pos_;
            ++current_column_;
            if (pos_ < end_) {
                if (find(compound_oper.begin(), compound_oper.end(), res + *pos_) != compound_oper.end()) {
                    res += *(pos_++);
                }
            }
        } else if (*pos_ == '\\') {
            type = Types::Literal;
            res += *(pos_++);
            ++current_column_;
        }
        return make_lexem(type, res, start_line, start_col);
    }
    return make_lexem(Types::END, "", start_line, start_col);
}

string type_to_string(Types type) {
    if (type == Types::Keyword) return "Keyword";
    else if (type == Types::Identificator) return "Identificator";
    else if (type == Types::Literal) return "Literal";
    else if (type == Types::Operation) return "Operation";
    else if (type == Types::Punctuation) return "Punctuation";
    else return "ELSE";
}