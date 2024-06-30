#include <random>
#include <cmath>

__int128_t pow_mod(long long p, long long n, long long m) // p^n % m
{
    __int128_t p_pow = p % m;
    long long cur = 1;
    while (n > 0) // binary exponentiation
    {
        if (n % 2)
        {
            cur = (cur * p_pow) % m;
        }
        n >>= 1;
        p_pow = (p_pow*p_pow) % m; // squaring
    }
    return cur;
}

double rand_num_gen(int i)
{
    static long long m = 4294967296;
    static long long a = 1664525;
    static long long c = 1;

    static long long z0 = rand() * m;
    long long p1 = (pow_mod(a, i, m)*z0) % m;
    long long p2 = ((((pow_mod(a, i, (m*(a-1))) - 1) % (m*(a-1))) / (a - 1)) * c) % m;
    long long z = (p1 + p2) % m;
    return (double)z / m;
}

double interp(double p1, double p2, double x)
{
    double x_rad = x*M_PI;
    double ratio = (1 - cos(x_rad)) * 0.5;
    // double ratio = x*x*x;
    return p1 * (1 - ratio) + p2 * ratio;
}

int main()
{
    srand(200);
    int x = 0, y = 15, amp = 20, wl = 5;
    double fq = 1/wl;
    int i = 1;
    double a = rand_num_gen(i++);
    double b = rand_num_gen(i++);
    while (x < 50)
    {
        if (x % wl == 0)
        {
            a = b;
            b = rand_num_gen(i++);
            y = 15 + a * amp;
            printf("\n%d ", y);
        }
        else
        {
            y = 15 + interp(a, b, (double) (x % wl) / wl) * amp;
            printf("%d ", y);
        }
        x += 1;
    }
    printf("\n");
}