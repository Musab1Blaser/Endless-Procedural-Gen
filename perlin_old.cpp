#include <random>
#include <cmath>

double rand_num_gen()
{
    static long long m = 4294967296;
    static long long a = 1664525;
    static long long c = 1;

    static long long z = rand() * m;
    z = (a * z + c) % m;
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
    srand(20);
    int x = 0, y = 15, amp = 20, wl = 5;
    double fq = 1/wl;
    double a = rand_num_gen();
    double b = rand_num_gen();
    while (x < 50)
    {
        if (x % wl == 0)
        {
            a = b;
            b = rand_num_gen();
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