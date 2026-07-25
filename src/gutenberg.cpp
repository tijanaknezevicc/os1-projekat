#include "../h/gutenberg.hpp"
#include  "../lib/console.h"

void print_str(const char *str) {
    while (*str) {
        __putc(*str);
        str++;
    }
}

void print_x(uint64 val) {
    const char* digits = "0123456789abcdef";
    print_str("0x");
    for (int i = 60; i >= 0; i -= 4) {
        int digitVal = (val >> i) & 0xF;
        __putc(digits[digitVal]);
    }
}