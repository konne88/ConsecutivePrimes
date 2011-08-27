#include <iostream>
#include <assert.h>

// http://www.generic-programming.org/~dgregor/cpp/variadic-templates.html

/** 
 * T type of prime
 * i internal iterator, init to 2
 * p prime number to check
 */
template<class T, int i, int p> class CheckPrime;

/**
 * Checks the prime p of type T with iterator i if s is true.
 */
template<class T, bool s,int i, int p>
class ContinuePrimeCheckIfTrue {
public:
    enum {
        isPrime = CheckPrime<T,i,p>::isPrime
    };
};

template<class T, int i, int p>
class ContinuePrimeCheckIfTrue<T,false,i,p> {
public:
    enum { isPrime = true };
};

template<class T, int i, int p>
class CheckPrime {
public:
    enum {
        isPrime = (p%i) && ContinuePrimeCheckIfTrue<T,i*i<=p,i+1,p>::isPrime 
    };
};

template<class T, int i>
class CheckPrime<T,i,0> {
public:
    enum { isPrime = false };
};

template<class T, int i>
class CheckPrime<T,i,1> {
public:
    enum { isPrime = false };
};

template<class T, int i>
class CheckPrime<T,i,2> {
public:
    enum { isPrime = true };
};

template<int p>
inline bool isPrime() {
    return CheckPrime<int,2,p>::isPrime;
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
 * T is type of primes
 * i is implemenation and the current element in the array, init to 0
 * p is the current number checked for primarity (init to 2)
 * n is the amount of primes that should be found
 * args are prime numbers already found (in other words i)
 */
template<class T, int i, int p, int n, int...args>
class Primes : public PickType<CheckPrime<T,2,p>::isPrime,
        Primes<T,i+1,p+1,n,args...,p>,
        Primes<T,i,p+1,n,args...>
    >::type
{};

template<class T, int p, int n, int...args>
class Primes<T,n,p,n,args...> {
public:
    static int array[sizeof...(args)];
};

// this describes the instance of the array in memory
template<class T, int p, int n, int...args>
int Primes<T,n,p,n,args...>::array[sizeof...(args)] = {args...};

int main() {
    for(int i=0 ; i<10 ; ++i){
        std::cerr << i << ' ' << Primes<int,0,1,10>::array[i] << '\n';
    }
    

    /*
    {
        assert(isPrime<0>() == false);
        assert(isPrime<1>() == false);
        assert(isPrime<2>() == true);
        assert(isPrime<3>() == true);
        assert(isPrime<4>() == false);
        assert(isPrime<5>() == true);
        assert(isPrime<6>() == false);
        assert(isPrime<7>() == true);
        assert(isPrime<8>() == false);
        assert(isPrime<9>() == false);

        const int count = 25;
        int* a = initFirstNPrimes<int,count>();
        int sum = 0;
        for(int i=0 ; i<count ; ++i) {
            sum += a[i];
        }
        assert(sum == 1060);
    }
    
    std::cerr << isPrime<3571>() << '\n';
    
    const int count = 500;
    int* a = initFirstNPrimes<int,count>();
    for(int i=0 ; i<count ; ++i) {
        std::cerr << a[i] << '\n';
    }
    
    
    
    return 0; */
}

/*

g++: internal compiler error: Killed (program cc1plus)
Please submit a full bug report,
with preprocessed source if appropriate.
See <https://bugs.archlinux.org/> for instructions.
g++ simple-prime-template.cpp -o template-test -ftemplate-depth=10000000  30.11s user 2.65s system 84% cpu 38.848 total

[151849.217913] Out of memory: Kill process 10260 (cc1plus) score 744 or sacrifice child
[151849.217916] Killed process 10260 (cc1plus) total-vm:2997188kB, anon-rss:2938232kB, file-rss:836kB

*/
