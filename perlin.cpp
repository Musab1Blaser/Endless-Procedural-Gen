#include <random>
#include <cmath>
#include "perlin.hpp"

Perlin::Perlin(int octaves, long long NUM_CHUNKS, long long CHUNK_SIZE)
{
    srand(0);
    for (int i = 0; i < octaves; i++)
        octave_z0.push_back(rand() % m);
    this->NUM_CHUNKS = NUM_CHUNKS;
    this->CHUNK_SIZE = CHUNK_SIZE;
}

__int128_t Perlin::pow_mod(long long p, long long n, long long m) // p^n % m
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

double Perlin::rand_num_gen(int octave, long long i)
{
    long long z0 = octave_z0[octave];
    long long p1 = (pow_mod(a, i, m)*z0) % m;
    long long p2 = ((((pow_mod(a, i, (m*(a-1))) - 1) % (m*(a-1))) / (a - 1)) * c) % m;
    long long z = (p1 + p2) % m;
    return (double)z / m;
}

double Perlin::interp(double p1, double p2, double x)
{
    double x_rad = x*M_PI;
    double ratio = (1 - cos(x_rad)) * 0.5;
    // double ratio = x*x*x;
    return p1 * (1 - ratio) + p2 * ratio;
}

std::vector<int> Perlin::gen_octave(int octave, int start_x, int y, long long amp, long long wl, long long num_blocks) 
{
    std::vector<int> cur_octave_heights;
    double fq = 1/wl;

    // fill in left corner of chunk
    int i = ((NUM_CHUNKS*CHUNK_SIZE + start_x - wl/2) % NUM_CHUNKS*CHUNK_SIZE) / wl + 1; // left most keypoint has i = 1
    double a = rand_num_gen(octave, i++);
    double b = rand_num_gen(octave, i++);

     
    int x = start_x; // left corner of chunk
    while (x < start_x + num_blocks)
    {
        if ((x+1-wl/2) % wl == 0) // once end of chunk
        {
            cur_octave_heights.push_back(y + b*amp);
            a = b;
            i = i % ((NUM_CHUNKS*CHUNK_SIZE)/wl) + 1; // move to next - loop around if required 
            b = rand_num_gen(octave, i++);
        }
        else
        {
            cur_octave_heights.push_back(y + interp(a, b, (double) ((x+1-wl/2) % wl) / wl) * amp);
        }
        x++;
    }
    return cur_octave_heights;
}

std::vector<int> Perlin::proc_gen_heights(const int CHUNK_SIZE, const int FIRST_CHUNK, const int NUM_CHUNKS)
{
    int amp = 16;
    std::vector<int> heights (CHUNK_SIZE*NUM_CHUNKS, 25); // use mean value of 25
    int wl = CHUNK_SIZE;
    for (int i = 0; i < octave_z0.size(); i++)
    {
        std::vector<int> cur_octave = gen_octave(i, FIRST_CHUNK*CHUNK_SIZE, 0, amp, wl, CHUNK_SIZE*NUM_CHUNKS);
        amp = amp/2;
        wl = wl/2;
        for (int j = 0; j < CHUNK_SIZE*NUM_CHUNKS; j++)
            heights[j] += cur_octave[j];
    }
    return heights;
}