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

    // --- ДОБАВЛЕННЫЕ МЕТОДЫ ДЛЯ ДОСТУПА ИЗВНЕ ---
    int get_degree() const { return size - 1; }
    T get_coef(int i) const
    {
        if (i < 0 || i >= size) return T{};
        return a[i];
    }
    // --------------------------------------------

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

// ==========================================================
// РЕШЕНИЕ ЗАДАЧ (ВНЕ КЛАССА, СОГЛАСНО ТРЕБОВАНИЯМ)
// ==========================================================

// Задача 3: Производная многочлена
template <typename T>
Polynomial<double> derivative(const Polynomial<T>& p) {
    int deg = p.get_degree();
    if (deg == 0) return Polynomial<double>(0);

    Polynomial<double> res(deg - 1);
    for (int i = 1; i <= deg; ++i) {
        res.set(i - 1, p.get_coef(i) * i);
    }
    res.shrink_to_fit();
    return res;
}
template <typename T>
Polynomial<double> calculate_integral(const Polynomial<T>& p) {
    int deg = p.get_degree();
    Polynomial<double> res(deg + 1);
    for (int i = 0; i <= deg; ++i) {
        res.set(i + 1, p.get_coef(i) / (i + 1.0));
    }
    res.shrink_to_fit();
    return res;
}
int main()
{
    std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<double> dist(-5.0, 5.0);

    Polynomial<double> p(3);
    Polynomial<double> q(2);

    for (int i = 0; i < 4; i++) p.set(i, dist(gen));
    for (int i = 0; i < 3; i++) q.set(i, dist(gen));

    std::cout << "First polynomial:\n" << p << "\n\n";
    std::cout << "Second polynomial:\n" << q << "\n\n";
    std::cout << "Addition:\n" << p + q << "\n\n";
    std::cout << "Subtraction:\n" << p - q << "\n\n";
    std::cout << "Multiplication:\n" << p * 2.0 << "\n\n";

    std::cout << "================ РЕШЕНИЕ ЗАДАЧ (ВАРИАНТ 3) ================\n\n";

    std::cout << "3. Производная многочлена p:\n";
    std::cout << derivative(p) << "\n\n";

    std::cout << "4. Неопределенный интеграл многочлена p:\n";
    std::cout << calculate_integral(p) << " + C\n\n";

    Polynomial<double> eq(2);
    eq.set(0, 1.0);
    eq.set(1, -3.0);
    eq.set(2, 2.0);
    std::cout << "Тестовое уравнение 2-й степени: " << eq << " = 0\n";

    double a = eq.get_coef(2), b = eq.get_coef(1), c = eq.get_coef(0);

    double D = b * b - 4 * a * c;
    if (D > 0) {
        std::cout << "1,2. Корни (степень 2): x1 = " << (-b + std::sqrt(D)) / (2 * a)
            << ", x2 = " << (-b - std::sqrt(D)) / (2 * a) << "\n";
    }
    else if (D == 0) {
        std::cout << "1,2. Корень (степень 2): x = " << -b / (2 * a) << "\n";
    }
    else {
        std::cout << "1,2. Действительных корней нет\n";
    }

    double x_ext = -b / (2 * a);
    double y_ext = eq(x_ext);
    std::cout << "5. Координаты экстремума: x = " << x_ext << ", y = " << y_ext << "\n\n";

    return 0;
}