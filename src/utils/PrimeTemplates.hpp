#include <iostream>
#include <assert.h>

// http://www.generic-programming.org/~dgregor/cpp/variadic-templates.html

// g++ -std=c++0x variadic-prime-template-better.cpp -o template-test -ftemplate-depth=1000000;    

typedef uint64_t buint;    // Biggest unsigned integer

/** 
 * i internal iterator, init to 2
 * p prime number to check
 */
template<buint p, buint i> class CheckPrime;

/**
 * Checks the prime p of type T with iterator i if s is true.
 */
template<bool s,buint p, buint i>
struct ContinuePrimeCheckIfTrue {
    enum { isPrime = CheckPrime<p,i>::isPrime };
};

template<buint p, buint i>
struct ContinuePrimeCheckIfTrue<false,p,i> {
    enum { isPrime = true };
};

template<buint p, buint i = 2>
struct CheckPrime {
    enum {
        isPrime = (p%i) && ContinuePrimeCheckIfTrue<i*i<=p,p,i+1>::isPrime 
    };
};

template<buint i>
struct CheckPrime<0,i> {
    enum { isPrime = false };
};

template<buint i>
struct CheckPrime<1,i> {
    enum { isPrime = false };
};

template<buint i>
struct CheckPrime<2,i> {
    enum { isPrime = true };
};

template<buint p>
inline bool isPrime() {
    return CheckPrime<p>::isPrime;
}

/**
 * if s is true, type is T otherwise F
 */
template<bool s, class T, class F>
struct PickType {
    typedef T type;
};

template<class T, class F>
struct PickType<false,T,F> {
    typedef F type;
};

/**
 * T is type of primes in the array
 * i is implemenation and the current element in the array, init to 0
 * p is the current number checked for primarity (init to 2)
 * n is the amount of primes that should be found
 * args are prime numbers already found (in other words i)
 */
template<class T, buint n, buint p = 2, buint i = 0, buint...args>
struct Primes : public PickType<CheckPrime<p>::isPrime,
        Primes<T,n,p+1,i+1,args...,p>,
        Primes<T,n,p+1,i,args...>
    >::type
{};

template<class T, buint n, buint p, buint...args>
struct Primes<T,n,p,n,args...> {
    static int array[n];
};

// this describes the instance of the array in memory
template<class T, buint n, buint p, buint...args>
int Primes<T,n,p,n,args...>::array[n] = {args...};

/*

g++: internal compiler error: Killed (program cc1plus)
Please submit a full bug report,
with preprocessed source if appropriate.
See <https://bugs.archlinux.org/> for instructions.
g++ simple-prime-template.cpp -o template-test -ftemplate-depth=10000000  30.11s user 2.65s system 84% cpu 38.848 total

[151849.217913] Out of memory: Kill process 10260 (cc1plus) score 744 or sacrifice child
[151849.217916] Killed process 10260 (cc1plus) total-vm:2997188kB, anon-rss:2938232kB, file-rss:836kB

*/
