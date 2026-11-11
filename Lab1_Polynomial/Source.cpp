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

    Polynomial& operator=(const Polynomial& p)
    {
        if (this != &p)
        {
            delete[] a;

            size = p.size;
            a = new T[size];

            for (int i = 0; i < size; i++)
                a[i] = p.a[i];
        }

        return *this;
    }

    int get_degree() const { return size - 1; }
    T get_coef(int i) const
    {
        if (i < 0 || i >= size) return T{};
        return a[i];
    }

    T& operator[](int i)
    {
        if (i < 0 || i >= size)
            throw std::out_of_range("Wrong index");

        return a[i];
    }

    void set(int i, T value)
    {
        if (i < 0)
            throw std::out_of_range("Wrong degree");

        if (i >= size)
            expand(i);

        a[i] = value;
    }

    void expand(int n)
    {
        if (n < size)
            return;

        T* b = new T[n + 1]{};

        for (int i = 0; i < size; i++)
            b[i] = a[i];

        delete[] a;

        a = b;
        size = n + 1;
    }

    void shrink_to_fit()
    {
        int n = size;

        while (n > 1 && std::abs(a[n - 1]) < 1e-9)
            n--;

        if (n != size)
        {
            T* b = new T[n];

            for (int i = 0; i < n; i++)
                b[i] = a[i];

            delete[] a;
            a = b;
            size = n;
        }
    }

    Polynomial operator+(const Polynomial& p) const
    {
        int n = size > p.size ? size : p.size;
        Polynomial result(n - 1);

        for (int i = 0; i < n; i++)
        {
            T x = i < size ? a[i] : T{};
            T y = i < p.size ? p.a[i] : T{};

            result.a[i] = x + y;
        }

        result.shrink_to_fit();
        return result;
    }

    Polynomial operator-(const Polynomial& p) const
    {
        int n = size > p.size ? size : p.size;
        Polynomial result(n - 1);

        for (int i = 0; i < n; i++)
        {
            T x = i < size ? a[i] : T{};
            T y = i < p.size ? p.a[i] : T{};

            result.a[i] = x - y;
        }

        result.shrink_to_fit();
        return result;
    }

    Polynomial operator*(T k) const
    {
        Polynomial result(*this);

        for (int i = 0; i < size; i++)
            result.a[i] *= k;

        result.shrink_to_fit();
        return result;
    }

    auto operator()(X x) const
    {
        using R = decltype(T{}* x);
        R result{};

        for (int i = size - 1; i >= 0; i--)
            result = result * x + a[i];

        return result;
    }

    bool operator==(const Polynomial& p) const
    {
        int n = size > p.size ? size : p.size;

        for (int i = 0; i < n; i++)
        {
            T x = i < size ? a[i] : T{};
            T y = i < p.size ? p.a[i] : T{};

            if (std::abs(x - y) > 1e-9)
                return false;
        }

        return true;
    }

    bool operator!=(const Polynomial& p) const
    {
        return !(*this == p);
    }

    friend std::ostream& operator<<(std::ostream& out,
        const Polynomial& p)
    {
        for (int i = p.size - 1; i >= 0; i--)
        {
            out << p.a[i] << "*x^" << i;

            if (i > 0)
                out << " + ";
        }

        return out;
    }

    friend std::istream& operator>>(std::istream& in,
        Polynomial& p)
    {
        for (int i = 0; i < p.size; i++)
            in >> p.a[i];

        return in;
    }
};