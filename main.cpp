#include "src/lexicalAnalyzer/lexer.cxx"
#include "src/lexicalAnalyzer/trie.cxx"
#include "src/syntaxAnalyzer/syntaxer.cxx"
#include "src/generation/poliz.cpp"
#include "src/generation/interpreter.cpp"

int main() {

    try {
        Trie trie;
        std::ifstream in_key("KeyWords.txt", std::ios::binary);
        loadFromfile(in_key, trie);
        Lexer lexer("Program.txt", trie);
        tf Tf;
        Syntaxer syntaxer(lexer, Tf);
        Poliz poliz = syntaxer.syntax();
        /*poliz.print_poliz(); */
        Interpreter interpreter(poliz, Tf);
        interpreter.runtime();
    } catch (pair<Lexem, pair<Types, string>> error) {
        std::cerr << type_to_string(error.first.type) << " " << error.first.value << '\n'
            << "in line:" << error.first.line << " column:" << error.first.column << '\n'
            << "need " << type_to_string(error.second.first) << " " << error.second.second;
    } catch (pair<Lexem, Types> error) {
        std::cerr << type_to_string(error.first.type) << " " << error.first.value << '\n'
            << "in line:" << error.first.line << " column:" << error.first.column << '\n'
            << "need " << type_to_string(error.second);
    } catch (Lexem error) {
        std::cerr << "where type in line: " << error.line << " column:" << error.column;
    } catch (string e) {
        std::cerr << e;
    } catch (const std::logic_error& e) {
        std::cerr << e.what() << '\n';
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << '\n';
    }
}