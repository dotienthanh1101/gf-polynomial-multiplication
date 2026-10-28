#include "gf2n.hpp"
#include <iostream>
#include <limits>

static std::uint32_t parse_number(const std::string& text) {
    unsigned base = 10;
    std::size_t start = 0;
    if (text.size() > 2 && text[0] == '0') {
        if (text[1] == 'x' || text[1] == 'X') { base = 16; start = 2; }
        else if (text[1] == 'b' || text[1] == 'B') { base = 2; start = 2; }
    }
    if (start == text.size()) throw std::invalid_argument("So rong.");
    std::uint32_t result = 0;
    for (std::size_t i = start; i < text.size(); ++i) {
        const char c = text[i];
        unsigned digit = 99;
        if (c >= '0' && c <= '9') digit = c - '0';
        else if (c >= 'a' && c <= 'f') digit = c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') digit = c - 'A' + 10;
        if (digit >= base || result > (std::numeric_limits<std::uint32_t>::max() - digit) / base)
            throw std::invalid_argument("So khong hop le hoac vuot mien uint32: " + text);
        result = result * base + digit;
    }
    return result;
}

static void run(unsigned n, std::uint32_t a, std::uint32_t b,
                std::uint32_t modulus, bool trace) {
    const gf::Field field(n, modulus);
    std::cout << "GF(2^" << n << "), m(x) = " << gf::polynomial(modulus)
              << " (" << gf::hex_value(modulus) << ")\n"
              << "a(x) = " << gf::polynomial(a) << '\n'
              << "b(x) = " << gf::polynomial(b) << '\n';
    const auto result = field.multiply(a, b, trace ? &std::cout : nullptr);
    std::cout << "Ket qua: " << gf::hex_value(a) << " * " << gf::hex_value(b)
              << " = " << gf::hex_value(result) << " = " << result << '\n'
              << "c(x) = " << gf::polynomial(result) << "\n\n";
}

static void usage(const char* name) {
    std::cout << "Cach dung: " << name << " n a b [modulus] [--trace]\n"
              << "n: 4 hoac 8; a,b: thap phan, 0xHEX hoac 0bBIN.\n"
              << "Modulo mac dinh: n=4 -> 0x13; n=8 -> 0x11B.\n"
              << "Khong truyen doi so: chay hai vi du mau.\n";
}

int main(int argc, char* argv[]) {
    try {
        if (argc == 1) {
            run(4, 0xB, 0x7, 0x13, false);
            run(8, 0x57, 0x83, 0x11B, false);
            return 0;
        }
        if (argc == 2 && std::string(argv[1]) == "--help") {
            usage(argv[0]); return 0;
        }
        if (argc < 4 || argc > 6) { usage(argv[0]); return 1; }
        const auto n = parse_number(argv[1]);
        if (n != 4 && n != 8)
            throw std::invalid_argument("n phai bang 4 hoac 8.");
        const auto a = parse_number(argv[2]), b = parse_number(argv[3]);
        std::uint32_t modulus = n == 4 ? 0x13 : 0x11B;
        bool trace = false, has_modulus = false;
        for (int i = 4; i < argc; ++i) {
            if (std::string(argv[i]) == "--trace" && !trace) trace = true;
            else if (!has_modulus && std::string(argv[i]) != "--trace") {
                modulus = parse_number(argv[i]); has_modulus = true;
            } else throw std::invalid_argument("Doi so bi lap hoac khong hop le.");
        }
        run(n, a, b, modulus, trace);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Loi: " << e.what() << '\n';
        return 1;
    }
}
