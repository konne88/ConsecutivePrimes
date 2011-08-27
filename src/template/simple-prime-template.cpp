#include <iostream>

template<class T, int p, int i>
class CheckPrime {
public:
    enum {
        isPrime = (p%i) && CheckPrime<T,p,i-1>::isPrime
    };
};

template<class T, int p>
class CheckPrime<T,p,0> {
public:
    enum { isPrime = true };
};

template<class T, int p>
class CheckPrime<T,p,1> {
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

template<class T, int p, int i, int n>
class GenPrimes {
public:
    static inline void gen(T(&a)[n]) {
        DoIfTrue<T, isPrime, i, p, n>::setArrayElement(a);
        GenPrimes<T,p+1,i+isPrime,n>::gen(a);
    }
    
    enum { isPrime = CheckPrime<T,p,p-1>::isPrime };
};

template<class T, int p, int n>
class GenPrimes<T,p,n,n> {
public:
    static inline void gen(T(&a)[n]) {}
};

template<class T,int n> 
T* initFirstNPrimes() {
    static T a[n];
    GenPrimes<T,1,0,n>::gen(a);
    return a;
}

template<int p>
inline bool isPrime() {
    return CheckPrime<int,p,p-1>::isPrime;
}

template<>
inline bool isPrime<0>() {
    return false;
}

int main() {
    isPrime<0>() == false;
    isPrime<1>() == true;
    isPrime<2>() == true;
    isPrime<3>() == true;
    isPrime<4>() == false;
    isPrime<5>() == true;
    isPrime<6>() == false;
    isPrime<7>() == true;
    isPrime<8>() == false;

    const int count = 20;
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
