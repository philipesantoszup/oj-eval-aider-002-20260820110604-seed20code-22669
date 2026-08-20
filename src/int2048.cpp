#include "int2048.h"
#include <complex>
#include <cmath>
#include <algorithm>
#include <string>

namespace sjtu {

using cd = std::complex<double>;
const double PI = std::acos(-1.0);

void fft(std::vector<cd>& a, bool invert) {
    int n = a.size();
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j)
            std::swap(a[i], a[j]);
    }

    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2 * PI / len * (invert ? -1 : 1);
        cd wlen(std::cos(ang), std::sin(ang));
        for (int i = 0; i < n; i += len) {
            cd w(1);
            for (int j = 0; j < len / 2; j++) {
                cd u = a[i + j], v = a[i + j + len/2] * w;
                a[i + j] = u + v;
                a[i + j + len/2] = u - v;
                w *= wlen;
            }
        }
    }

    if (invert) {
        for (cd& x : a)
            x /= n;
    }
}

const long long int2048::BASE;

void int2048::normalize() {
    while (mag.size() > 1 && mag.back() == 0) {
        mag.pop_back();
    }
    if (is_negative && mag.size() == 1 && mag[0] == 0) {
        is_negative = false;
    }
}

int int2048::compare_mag(const int2048& a, const int2048& b) {
    if (a.mag.size() != b.mag.size()) {
        return a.mag.size() > b.mag.size() ? 1 : -1;
    }
    for (int i = a.mag.size() - 1; i >= 0; --i) {
        if (a.mag[i] != b.mag[i]) {
            return a.mag[i] > b.mag[i] ? 1 : -1;
        }
    }
    return 0;
}

int2048 int2048::add_mag(const int2048& a, const int2048& b) {
    int2048 res;
    res.is_negative = false;
    res.mag.clear();
    long long carry = 0;
    int max_len = std::max(a.mag.size(), b.mag.size());
    for (int i = 0; i < max_len || carry; ++i) {
        long long sum = carry;
        if (i < a.mag.size()) sum += a.mag[i];
        if (i < b.mag.size()) sum += b.mag[i];
        res.mag.push_back(sum % BASE);
        carry = sum / BASE;
    }
    res.normalize();
    return res;
}

int2048 int2048::subtract_mag(const int2048& a, const int2048& b) {
    int2048 res;
    res.is_negative = false;
    res.mag.clear();
    long long borrow = 0;
    for (int i = 0; i < a.mag.size(); ++i) {
        long long digit_a = a.mag[i] - borrow;
        long long digit_b = (i < b.mag.size()) ? b.mag[i] : 0;
        if (digit_a < digit_b) {
            digit_a += BASE;
            borrow = 1;
        } else {
            borrow = 0;
        }
        res.mag.push_back(digit_a - digit_b);
    }
    res.normalize();
    return res;
}

int2048 int2048::multiply_mag(const int2048& a, const int2048& b) {
    if ((a.mag.size() == 1 && a.mag[0] == 0) || (b.mag.size() == 1 && b.mag[0] == 0)) {
        int2048 res;
        res.is_negative = false;
        res.mag = {0};
        return res;
    }

    std::vector<long long> a_1e5, b_1e5;
    for (long long num : a.mag) {
        a_1e5.push_back(num % 100000LL);
        a_1e5.push_back(num / 100000LL);
    }
    while (a_1e5.size() > 1 && a_1e5.back() == 0) {
        a_1e5.pop_back();
    }
    for (long long num : b.mag) {
        b_1e5.push_back(num % 100000LL);
        b_1e5.push_back(num / 100000LL);
    }
    while (b_1e5.size() > 1 && b_1e5.back() == 0) {
        b_1e5.pop_back();
    }

    std::vector<cd> fa(a_1e5.begin(), a_1e5.end()), fb(b_1e5.begin(), b_1e5.end());
    int n = 1;
    while (n < a_1e5.size() + b_1e5.size()) {
        n <<= 1;
    }
    fa.resize(n);
    fb.resize(n);

    fft(fa, false);
    fft(fb, false);
    for (int i = 0; i < n; i++) {
        fa[i] *= fb[i];
    }
    fft(fa, true);

    std::vector<long long> res_1e5(n);
    for (int i = 0; i < n; i++) {
        res_1e5[i] = std::round(fa[i].real());
    }

    long long carry = 0;
    for (int i = 0; i < res_1e5.size() || carry; i++) {
        if (i < res_1e5.size()) {
            carry += res_1e5[i];
            res_1e5[i] = carry % 100000LL;
        } else {
            res_1e5.push_back(carry % 100000LL);
        }
        carry /= 100000LL;
    }

    while (res_1e5.size() > 1 && res_1e5.back() == 0) {
        res_1e5.pop_back();
    }

    // Build decimal string from base-1e5 digits
    std::string s;
    for (auto it = res_1e5.rbegin(); it != res_1e5.rend(); ++it) {
        std::string digit_str = std::to_string(*it);
        if (digit_str.size() < 5) {
            s += std::string(5 - digit_str.size(), '0');
        }
        s += digit_str;
    }
    // Remove leading zeros
    size_t start = 0;
    while (start < s.size() && s[start] == '0') {
        start++;
    }
    int2048 res;
    if (start == s.size()) {
        res = int2048(0);
    } else {
        res.read(s.substr(start));
    }
    res.is_negative = false;
    res.normalize();

    return res;
}

int2048 int2048::divide_mag(const int2048& dividend, const int2048& divisor, int2048& remainder) {
    int2048 quotient;
    quotient.is_negative = false;
    remainder = dividend;

    if (compare_mag(dividend, divisor) < 0) {
        quotient.mag = {0};
        return quotient;
    }

    int quotient_size = dividend.mag.size() - divisor.mag.size() + 1;
    quotient.mag.resize(quotient_size, 0);
    int k = divisor.mag.size();

    for (int i = quotient_size - 1; i >= 0; --i) {
        long long q = 0;
        int m = remainder.mag.size();
        int m_prime = m - i;

        if (m_prime > 0) {
            int t = std::min(k + 1, m_prime);
            long long R_top = 0;
            for (int j = 0; j < t; ++j) {
                int rem_digit_idx = (m - 1) - j;
                if (rem_digit_idx < i) {
                    break;
                }
                R_top = R_top * BASE + remainder.mag[rem_digit_idx];
            }

            long long D_top = 0;
            int d_take = std::min(k, t);
            for (int j = 0; j < d_take; ++j) {
                int div_digit_idx = (k - 1) - j;
                D_top = D_top * BASE + divisor.mag[div_digit_idx];
            }

            if (D_top != 0) {
                q = R_top / D_top;
                if (q < 0) q = 0;
                if (q >= BASE) q = BASE - 1;
            } else {
                q = BASE - 1;
            }

            while (q > 0) {
                int2048 q_num(q);
                int2048 product = multiply_mag(q_num, divisor);
                bool product_is_zero = (product.mag.size() == 1 && product.mag[0] == 0);
                if (!product_is_zero) {
                    product.mag.insert(product.mag.begin(), i, 0);
                }
                int cmp = compare_mag(product, remainder);
                if (cmp <= 0) {
                    break;
                }
                q--;
            }

            while (q < BASE - 1) {
                int2048 q_plus_1_num(q + 1);
                int2048 product = multiply_mag(q_plus_1_num, divisor);
                bool product_is_zero = (product.mag.size() == 1 && product.mag[0] == 0);
                if (!product_is_zero) {
                    product.mag.insert(product.mag.begin(), i, 0);
                }
                int cmp = compare_mag(product, remainder);
                if (cmp <= 0) {
                    q++;
                } else {
                    break;
                }
            }
        }

        quotient.mag[i] = q;

        if (q != 0) {
            int2048 q_num(q);
            int2048 subtract_val = multiply_mag(q_num, divisor);
            if (!(subtract_val.mag.size() == 1 && subtract_val.mag[0] == 0)) {
                subtract_val.mag.insert(subtract_val.mag.begin(), i, 0);
            }
            remainder = subtract_mag(remainder, subtract_val);
        }
    }

    quotient.normalize();
    remainder.normalize();
    return quotient;
}

int2048::int2048() : is_negative(false), mag{0} {}

int2048::int2048(long long n) {
    if (n < 0) {
        is_negative = true;
        if (n == -9223372036854775807LL - 1) {
            read("-9223372036854775808");
            return;
        } else {
            n = -n;
        }
    } else {
        is_negative = false;
    }
    mag.clear();
    if (n == 0) {
        mag.push_back(0);
    } else {
        while (n > 0) {
            mag.push_back(n % BASE);
            n /= BASE;
        }
    }
    normalize();
}

int2048::int2048(const std::string& s) {
    read(s);
}

int2048::int2048(const int2048& other) : is_negative(other.is_negative), mag(other.mag) {}

void int2048::read(const std::string& s) {
    is_negative = false;
    mag.clear();
    size_t start = 0;
    if (s.empty()) {
        mag.push_back(0);
        normalize();
        return;
    }
    if (s[0] == '-') {
        is_negative = true;
        start = 1;
    } else if (s[0] == '+') {
        start = 1;
    }
    int len = s.size() - start;
    if (len == 0) {
        mag.push_back(0);
        normalize();
        return;
    }
    for (int i = len; i > 0; i -= 9) {
        int end_pos = start + i;
        int begin_pos = (i >= 9) ? (end_pos - 9) : start;
        std::string digit_str = s.substr(begin_pos, end_pos - begin_pos);
        long long digit = 0;
        for (char c : digit_str) {
            digit = digit * 10 + (c - '0');
        }
        mag.push_back(digit);
    }
    normalize();
}

void int2048::print() {
    if (is_negative) {
        putchar('-');
    }
    printf("%lld", mag.back());
    for (int i = mag.size() - 2; i >= 0; --i) {
        printf("%09lld", mag[i]);
    }
}

int2048& int2048::add(const int2048& other) {
    if (is_negative == other.is_negative) {
        int2048 sum = add_mag(*this, other);
        sum.is_negative = is_negative;
        *this = sum;
    } else {
        int cmp = compare_mag(*this, other);
        if (cmp == 0) {
            *this = int2048();
        } else if (cmp > 0) {
            int2048 diff = subtract_mag(*this, other);
            diff.is_negative = is_negative;
            *this = diff;
        } else {
            int2048 diff = subtract_mag(other, *this);
            diff.is_negative = !is_negative;
            *this = diff;
        }
    }
    normalize();
    return *this;
}

int2048 add(int2048 a, const int2048& b) {
    a.add(b);
    return a;
}

int2048& int2048::minus(const int2048& other) {
    int2048 neg_other = -other;
    add(neg_other);
    return *this;
}

int2048 minus(int2048 a, const int2048& b) {
    a.minus(b);
    return a;
}

int2048 int2048::operator+() const {
    return *this;
}

int2048 int2048::operator-() const {
    int2048 res = *this;
    if (!(res.mag.size() == 1 && res.mag[0] == 0)) {
        res.is_negative = !res.is_negative;
    }
    return res;
}

int2048& int2048::operator=(const int2048& other) {
    if (this != &other) {
        is_negative = other.is_negative;
        mag = other.mag;
    }
    return *this;
}

int2048& int2048::operator+=(const int2048& other) {
    return add(other);
}

int2048& int2048::operator-=(const int2048& other) {
    return minus(other);
}

int2048& int2048::operator*=(const int2048& other) {
    bool new_sign = (is_negative != other.is_negative);
    int2048 product = multiply_mag(*this, other);
    if (!(product.mag.size() == 1 && product.mag[0] == 0)) {
        product.is_negative = new_sign;
    }
    *this = product;
    normalize();
    return *this;
}

int2048& int2048::operator/=(const int2048& other) {
    if (mag.size() == 1 && mag[0] == 0) {
        return *this;
    }

    int2048 x_abs = *this;
    x_abs.is_negative = false;
    int2048 y_abs = other;
    y_abs.is_negative = false;

    int2048 remainder_abs;
    int2048 quotient_abs = divide_mag(x_abs, y_abs, remainder_abs);

    bool quotient_sign = (is_negative != other.is_negative);
    bool has_remainder = !(remainder_abs.mag.size() == 1 && remainder_abs.mag[0] == 0);

    if (has_remainder && quotient_sign) {
        quotient_abs = add_mag(quotient_abs, int2048(1));
    }

    *this = quotient_abs;
    if (!(mag.size() == 1 && mag[0] == 0)) {
        is_negative = quotient_sign;
    }
    normalize();
    return *this;
}

int2048& int2048::operator%=(const int2048& other) {
    int2048 quotient = *this / other;
    int2048 product = quotient * other;
    *this = *this - product;
    normalize();
    return *this;
}

int2048 operator+(int2048 a, const int2048& b) {
    a += b;
    return a;
}

int2048 operator-(int2048 a, const int2048& b) {
    a -= b;
    return a;
}

int2048 operator*(int2048 a, const int2048& b) {
    a *= b;
    return a;
}

int2048 operator/(int2048 a, const int2048& b) {
    a /= b;
    return a;
}

int2048 operator%(int2048 a, const int2048& b) {
    a %= b;
    return a;
}

std::istream& operator>>(std::istream& is, int2048& num) {
    std::string s;
    is >> s;
    num.read(s);
    return is;
}

std::ostream& operator<<(std::ostream& os, const int2048& num) {
    if (num.is_negative) {
        os << '-';
    }
    os << num.mag.back();
    for (int i = num.mag.size() - 2; i >= 0; --i) {
        long long digit = num.mag[i];
        std::string s = std::to_string(digit);
        if (s.size() < 9) {
            s = std::string(9 - s.size(), '0') + s;
        }
        os << s;
    }
    return os;
}

bool operator==(const int2048& a, const int2048& b) {
    if (a.is_negative != b.is_negative) {
        return false;
    }
    return int2048::compare_mag(a, b) == 0;
}

bool operator!=(const int2048& a, const int2048& b) {
    return !(a == b);
}

bool operator<(const int2048& a, const int2048& b) {
    if (a.is_negative != b.is_negative) {
        return a.is_negative;
    }
    if (a.is_negative) {
        return int2048::compare_mag(a, b) > 0;
    } else {
        return int2048::compare_mag(a, b) < 0;
    }
}

bool operator>(const int2048& a, const int2048& b) {
    return b < a;
}

bool operator<=(const int2048& a, const int2048& b) {
    return !(b < a);
}

bool operator>=(const int2048& a, const int2048& b) {
    return !(a < b);
}

} // namespace sjtu
