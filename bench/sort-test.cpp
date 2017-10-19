#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
#include <cpp-sort/sorters.h>
#include <tight_pair.h>

#ifdef _WIN32
    #include <intrin.h>
    #define rdtsc __rdtsc
#else
    #ifdef __i586__
        static __inline__ unsigned long long rdtsc() {
            unsigned long long int x;
            __asm__ volatile(".byte 0x0f, 0x31" : "=A" (x));
            return x;
        }
    #elif defined(__x86_64__)
        static __inline__ unsigned long long rdtsc(){
            unsigned hi, lo;
            __asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
            return ((unsigned long long) lo) | (((unsigned long long) hi) << 32);
        }
    #else
        #error no rdtsc implementation
    #endif
#endif

template<typename Derived>
struct distribution
{
    template<typename OutputIterator>
    using fptr_t = void(*)(OutputIterator, std::size_t);

    template<typename OutputIterator>
    operator fptr_t<OutputIterator>() const
    {
        return [](OutputIterator out, std::size_t size)
        {
            return Derived{}(out, size);
        };
    }
};

std::mt19937_64 engine{};

template<typename T>
struct shuffled:
    distribution<shuffled<T>>
{
    template<typename OutputIterator>
    auto operator()(OutputIterator out, std::size_t size) const
        -> void
    {
        using std::get;

        // Pseudo-random number generator
       // thread_local std::mt19937_64 engine(seed);

        std::vector<T> vec;
        for (std::size_t i = 0 ; i < size ; ++i) {
            vec.emplace_back(i % 16, 0);
        }
        std::shuffle(std::begin(vec), std::end(vec), engine);
        for (std::size_t i = 0 ; i < size ; ++i) {
            get<1>(vec[i]) = i;
        }
        std::shuffle(std::begin(vec), std::end(vec), engine);
        std::move(std::begin(vec), std::end(vec), out);
    }

    static constexpr const char* output = "shuffled.txt";
};

template<template<typename...> class Collection, typename T>
using distr_f = void (*)(std::back_insert_iterator<Collection<T>>, std::size_t);

template<template<typename...> class Collection, typename T>
using sort_f = void (*)(Collection<T>&, std::less<>);

int main()
{
    using namespace std::chrono_literals;

    std::tuple<
        std::string,
        distr_f<std::vector, std::pair<unsigned short, unsigned short>>,
        distr_f<std::vector, cruft::tight_pair<unsigned short, unsigned short>>
    > distributions[] = {
        { "shuffled", shuffled<std::pair<unsigned short, unsigned short>>(), shuffled<cruft::tight_pair<unsigned short, unsigned short>>() },
    };

    std::tuple<
        std::string,
        sort_f<std::vector, std::pair<unsigned short, unsigned short>>,
        sort_f<std::vector, cruft::tight_pair<unsigned short, unsigned short>>
    > sorts[] = {
        { "heap_sort",      cppsort::heap_sort, cppsort::heap_sort      },
        { "pdq_sort",       cppsort::pdq_sort, cppsort::pdq_sort       },
        { "quick_sort",     cppsort::quick_sort, cppsort::quick_sort     },
        { "std_sort",       cppsort::std_sort, cppsort::std_sort       },
        { "verge_sort",     cppsort::verge_sort, cppsort::verge_sort     }
    };

    std::size_t sizes[] = { 1'000'000 };

    using std::get;

    for (auto& distribution: distributions) {
        for (auto& sort: sorts) {
            for (auto size: sizes) {
                // Common seed sequence to make sure both have the same seed
                std::seed_seq sseq{45518, 546312, 510};

                // Sort collection of std::pair
                {
                    engine.seed(sseq);

                    std::vector<std::uint64_t> cycles;

                    auto total_start = std::chrono::high_resolution_clock::now();
                    auto total_end = std::chrono::high_resolution_clock::now();
                    while (std::chrono::duration_cast<std::chrono::seconds>(total_end - total_start) < 5s) {
                        std::vector<std::pair<unsigned short, unsigned short>> collection;
                        get<1>(distribution)(std::back_inserter(collection), size);
                        std::uint64_t start = rdtsc();
                        get<1>(sort)(collection, std::less{});
                        std::uint64_t end = rdtsc();
                        assert(std::is_sorted(std::begin(collection), std::end(collection)));
                        cycles.push_back(double(end - start) / size + 0.5);
                        total_end = std::chrono::high_resolution_clock::now();
                    }

                    std::sort(std::begin(cycles), std::end(cycles));

                    std::cout << "std " << get<0>(distribution) << ' ' << get<0>(sort)
                              << ' ' << cycles[cycles.size() / 2] << '\n';
                    std::cerr << "std " << get<0>(distribution) << ' ' << get<0>(sort)
                              << ' ' << cycles[cycles.size() / 2] << '\n';
                }

                // Sort collection of cruft::tight_pair
                {
                    engine.seed(sseq);

                    std::vector<std::uint64_t> cycles;

                    auto total_start = std::chrono::high_resolution_clock::now();
                    auto total_end = std::chrono::high_resolution_clock::now();
                    while (std::chrono::duration_cast<std::chrono::seconds>(total_end - total_start) < 5s) {
                        std::vector<cruft::tight_pair<unsigned short, unsigned short>> collection;
                        get<2>(distribution)(std::back_inserter(collection), size);
                        std::uint64_t start = rdtsc();
                        get<2>(sort)(collection, std::less{});
                        std::uint64_t end = rdtsc();
                        assert(std::is_sorted(std::begin(collection), std::end(collection)));
                        cycles.push_back(double(end - start) / size + 0.5);
                        total_end = std::chrono::high_resolution_clock::now();
                    }

                    std::sort(std::begin(cycles), std::end(cycles));

                    std::cout << "cruft " << get<0>(distribution) << ' ' << get<0>(sort)
                              << ' ' << cycles[cycles.size() / 2] << '\n';
                    std::cerr << "cruft " << get<0>(distribution) << ' ' << get<0>(sort)
                              << ' ' << cycles[cycles.size() / 2] << '\n';
                }
            }
        }
    }
}
