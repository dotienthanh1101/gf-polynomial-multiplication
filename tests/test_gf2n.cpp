#include "../gf2n.hpp"
#include <iostream>
#include <stdexcept>

static void check(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

// Cach doc lap: nhan day du truoc, sau do chia da thuc lay du.
// Khong su dung ham remainder hoac multiply cua chuong trinh.
static std::uint32_t reference(unsigned a, unsigned b, unsigned n, unsigned m) {
    std::uint32_t product = 0;
    for (unsigned i = 0; i < n; ++i)
        for (unsigned j = 0; j < n; ++j)
            if (((a >> i) & 1u) && ((b >> j) & 1u)) product ^= 1u << (i + j);
    for (int k = static_cast<int>(2 * n - 2); k >= static_cast<int>(n); --k)
        if (product & (1u << k)) product ^= m << (k - n);
    return product;
}

static void test_field(unsigned n, unsigned m) {
    const gf::Field f(n, m);
    const unsigned size = 1u << n;
    for (unsigned a = 0; a < size; ++a) {
        bool inverse_exists = a == 0;
        for (unsigned b = 0; b < size; ++b) {
            auto c = f.multiply(a, b);
            check(c < size, "Ket qua vuot mien");
            check(c == reference(a, b, n, m), "Sai so voi nhan roi chia lay du");
            check(c == f.multiply(b, a), "Sai tinh giao hoan");
            if (c == 1) inverse_exists = true;
        }
        check(inverse_exists, "Phan tu khac 0 khong co nghich dao");
        check(f.multiply(a, 0) == 0, "Sai phep nhan voi 0");
        check(f.multiply(a, 1) == a, "Sai phep nhan voi 1");
    }
    // Duyet moi bo ba o GF(16); lay mau xac dinh o GF(256).
    const unsigned stride = n == 4 ? 1 : 17;
    for (unsigned a = 0; a < size; a += stride)
        for (unsigned b = 0; b < size; b += stride)
            for (unsigned c = 0; c < size; c += stride) {
                check(f.multiply(a, b ^ c) == (f.multiply(a, b) ^ f.multiply(a, c)),
                      "Sai tinh phan phoi");
                check(f.multiply(f.multiply(a, b), c) == f.multiply(a, f.multiply(b, c)),
                      "Sai tinh ket hop");
            }
    std::cout << "PASS GF(2^" << n << ") modulo " << gf::hex_value(m)
              << ": " << size * size << " cap nhan.\n";
}

int main() {
    try {
        test_field(4, 0x13);
        test_field(8, 0x11B);
        test_field(8, 0x11D);
        check(gf::Field(4, 0x13).multiply(0xB, 7) == 4, "Sai vi du GF(16)");
        check(gf::Field(8, 0x11B).multiply(0x57, 0x83) == 0xC1, "Sai vi du GF(256)");
        check(!gf::is_irreducible(0x11F, 8), "Chap nhan da thuc kha quy 0x11F");
        for (const auto m : {0u, 0x11Fu, 0x13u}) {
            bool rejected = false;
            try { gf::Field invalid(8, m); }
            catch (const std::invalid_argument&) { rejected = true; }
            check(rejected, "Khong tu choi modulo sai");
        }
        bool rejected = false;
        try { gf::Field(4, 0x13).multiply(16, 1); }
        catch (const std::invalid_argument&) { rejected = true; }
        check(rejected, "Khong tu choi phan tu vuot mien");
        rejected = false;
        try { gf::Field invalid(5, 0x25); }
        catch (const std::invalid_argument&) { rejected = true; }
        check(rejected, "Khong tu choi bac truong sai");
        std::cout << "Tat ca kiem thu deu dat.\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAIL: " << e.what() << '\n'; return 1;
    }
}
