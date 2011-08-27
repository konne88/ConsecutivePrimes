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
    int v[] = {
        1,2,4,8,16,32,64,128,1337
    };

    std::cerr << isPrime<9>() << '\n';
    std::cerr << isPrime<6>() << '\n';
    std::cerr << isPrime<5>() << '\n';
    std::cerr << isPrime<4>() << '\n';
    std::cerr << isPrime<3>() << '\n';
    std::cerr << isPrime<2>() << '\n';
    std::cerr << isPrime<1>() << '\n';

    std::cerr << "----------\n";
    
    const int count = 100;
    int* a = initFirstNPrimes<int,count>();
    for(int i=0 ; i<count ; ++i) {
        std::cerr << a[i] << '\n';
    }
    return 0;
}

