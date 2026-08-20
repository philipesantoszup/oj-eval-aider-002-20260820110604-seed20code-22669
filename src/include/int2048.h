#pragma once
#ifndef SJTU_BIGINTEGER
#define SJTU_BIGINTEGER

#include <complex>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace sjtu {
class int2048 {
private:
    bool is_negative;
    std::vector<long long> mag;
    static const long long BASE = 1000000000LL;

    void normalize();
    static int compare_mag(const int2048& a, const int2048& b);
    static int2048 add_mag(const int2048& a, const int2048& b);
    static int2048 subtract_mag(const int2048& a, const int2048& b);
    static int2048 multiply_mag(const int2048& a, const int2048& b);
    static int2048 divide_mag(const int2048& dividend, const int2048& divisor, int2048& remainder);

public:
    int2048();
    int2048(long long);
    int2048(const std::string &);
    int2048(const int2048 &);

    void read(const std::string &);
    void print();

    int2048 &add(const int2048 &);
    friend int2048 add(int2048, const int2048 &);

    int2048 &minus(const int2048 &);
    friend int2048 minus(int2048, const int2048 &);

    int2048 operator+() const;
    int2048 operator-() const;

    int2048 &operator=(const int2048 &);

    int2048 &operator+=(const int2048 &);
    friend int2048 operator+(int2048, const int2048 &);

    int2048 &operator-=(const int2048 &);
    friend int2048 operator-(int2048, const int2048 &);

    int2048 &operator*=(const int2048 &);
    friend int2048 operator*(int2048, const int2048 &);

    int2048 &operator/=(const int2048 &);
    friend int2048 operator/(int2048, const int2048 &);

    int2048 &operator%=(const int2048 &);
    friend int2048 operator%(int2048, const int2048 &);

    friend std::istream &operator>>(std::istream &, int2048 &);
    friend std::ostream &operator<<(std::ostream &, const int2048 &);

    friend bool operator==(const int2048 &, const int2048 &);
    friend bool operator!=(const int2048 &, const int2048 &);
    friend bool operator<(const int2048 &, const int2048 &);
    friend bool operator>(const int2048 &, const int2048 &);
    friend bool operator<=(const int2048 &, const int2048 &);
    friend bool operator>=(const int2048 &, const int2048 &);
};
} // namespace sjtu

#endif
