#ifndef GF2N_HPP
#define GF2N_HPP

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace gf {

// Bit i la he so cua x^i. Vi du: 0x13 = x^4 + x + 1.
inline int degree(std::uint32_t p) {
    int d = -1;
    while (p != 0) { ++d; p >>= 1; }
    return d;
}

inline std::uint32_t remainder(std::uint32_t a, std::uint32_t b) {
    if (b == 0) throw std::invalid_argument("Khong the chia cho da thuc 0.");
    const int db = degree(b);
    while (a != 0 && degree(a) >= db)
        a ^= b << (degree(a) - db);
    return a;
}

inline bool is_irreducible(std::uint32_t p, unsigned n) {
    if ((n != 4 && n != 8) || degree(p) != static_cast<int>(n)) return false;
    // Da thuc kha quy bac n co it nhat mot uoc bac <= n/2.
    // Thu tat ca da thuc monic bac 1..n/2; chi dung cho n=4,8.
    for (unsigned d = 1; d <= n / 2; ++d)
        for (std::uint32_t q = 1u << d; q < (1u << (d + 1)); ++q)
            if (remainder(p, q) == 0) return false;
    return true;
}

inline std::string polynomial(std::uint32_t p) {
    if (p == 0) return "0";
    std::ostringstream out;
    bool first = true;
    for (int i = degree(p); i >= 0; --i) {
        if ((p & (1u << i)) == 0) continue;
        if (!first) out << " + ";
        if (i == 0) out << '1';
        else if (i == 1) out << 'x';
        else out << "x^" << i;
        first = false;
    }
    return out.str();
}

inline std::string hex_value(std::uint32_t value) {
    std::ostringstream out;
    out << "0x" << std::uppercase << std::hex << value;
    return out.str();
}

class Field {
public:
    Field(unsigned n, std::uint32_t modulus) : n_(n), modulus_(modulus) {
        if (n != 4 && n != 8)
            throw std::invalid_argument("Chi ho tro GF(2^4) va GF(2^8).");
        if (!is_irreducible(modulus, n))
            throw std::invalid_argument("Da thuc modulo phai bat kha quy va co bac n.");
    }

    unsigned bits() const { return n_; }
    std::uint32_t modulus() const { return modulus_; }

    std::uint32_t multiply(std::uint32_t a, std::uint32_t b,
                           std::ostream* trace = nullptr) const {
        const std::uint32_t mask = (1u << n_) - 1;
        if (a > mask || b > mask)
            throw std::invalid_argument("Phan tu phai nam trong [0, 2^n - 1].");
        std::uint32_t result = 0;
        if (trace) *trace << "i | a | b | b&1 | ket qua sau XOR | tran bac n\n";
        for (unsigned i = 0; i < n_; ++i) {
            const bool low_bit = (b & 1u) != 0;
            if (low_bit) result ^= a;  // Cong da thuc trong GF(2).
            const bool carry = (a & (1u << (n_ - 1))) != 0;
            if (trace)
                *trace << i << " | " << hex_value(a) << " | " << hex_value(b)
                       << " | " << low_bit << " | " << hex_value(result)
                       << " | " << carry << '\n';
            a <<= 1;                  // Nhan a(x) voi x.
            if (carry) a ^= modulus_; // Khu hang x^n bang m(x).
            a &= mask;
            b >>= 1;                  // Xet he so tiep theo cua b(x).
        }
        return result;
    }

private:
    unsigned n_;
    std::uint32_t modulus_;
};

} // namespace gf
#endif
