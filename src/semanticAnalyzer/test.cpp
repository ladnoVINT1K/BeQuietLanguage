#include "tid.cpp"
#include "stack.cpp"
#include <iostream>

int main() {
    type_stack A;
    A.push_stack("-");
    A.push_stack("123.123");

    try{ 
        A.check_uno();
        std::cout << A.check_if();
    } catch (std::exception &e) {
        std::cout << e.what();
    }
}