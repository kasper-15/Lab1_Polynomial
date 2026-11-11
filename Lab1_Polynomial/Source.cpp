#include <iostream>
#include <complex>
#include <cmath>
#include <random>
#include <stdexcept>
#include <type_traits>

template <typename T>
struct RealType
{
    using type = T;
};

template <typename T>
struct RealType<std::complex<T>>
{
    using type = T;
};

template <typename T, typename X = T>
class Polynomial
{
private:
    T* a;
    int size;

public:
    Polynomial(int n = 0)
    {
        if (n < 0)
            throw std::invalid_argument("Wrong degree");

        size = n + 1;
        a = new T[size]{};
    }

    Polynomial(const Polynomial& p)
    {
        size = p.size;
        a = new T[size];

        for (int i = 0; i < size; i++)
            a[i] = p.a[i];
    }

    ~Polynomial()
    {
        delete[] a;
    }
}