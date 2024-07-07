#include <vector>

class Perlin
{
    private:
        long long m = 4294967296;
        long long a = 1664525;
        long long c = 1;
        long long NUM_CHUNKS, CHUNK_SIZE;

        std::vector<long long> octave_z0;
        
        double rand_num_gen(int octave, long long i);
        __int128_t pow_mod(long long p, long long n, long long m); // p^n % m
        double interp(double p1, double p2, double x);
        std::vector<int> gen_octave(int octave, long long start_x, int y, long long amp, long long wl, long long num_blocks);

    public:
        Perlin(int octaves, long long NUM_CHUNKS, long long CHUNK_SIZE);
        std::vector<int> proc_gen_heights(const long long FIRST_CHUNK, const int CHUNKS_to_generate); 
};



