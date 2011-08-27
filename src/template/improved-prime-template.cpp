#include <iostream>
#include <assert.h>



template<class T, int i, int p> class CheckPrime;

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

template<class T, bool s, int i, int v, int n>
class DoIfTrue {
public:
    static inline void setArrayElement(T(&a)[n]) {}
};

template<class T, int i, int v, int n>
class DoIfTrue<T,true,i,v,n> {
public:
    static inline void setArrayElement(T(&a)[n]) {
        a[i] = v;
    }
};

template<class T, int i, int p, int n>
class GenPrimes {
public:
    static inline void gen(T(&a)[n]) {
        DoIfTrue<T, isPrime, i, p, n>::setArrayElement(a);
        GenPrimes<T,i+isPrime,p+1,n>::gen(a);
    }
    
    enum { isPrime = CheckPrime<T,2,p>::isPrime };
};

template<class T, int p, int n>
class GenPrimes<T,n,p,n> {
public:
    static inline void gen(T(&a)[n]) {}
};

template<class T,int n> 
T* initFirstNPrimes() {
    static T a[n];
    GenPrimes<T,0,1,n>::gen(a);
    return a;
}

template<int p>
inline bool isPrime() {
    return CheckPrime<int,2,p>::isPrime;
}

int main() {
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
    return 0;
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
