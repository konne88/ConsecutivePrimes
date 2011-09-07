/*
 * Sqrt.hpp
 *
 *  Created on: Aug 12, 2011
 *      Author: konne
 */

#ifndef SQRT_HPP_
#define SQRT_HPP_

/**
 * Calculates the next odd number greater or equal to n at compiletime
 */
template<int64_t n>
struct CeilToOdd {
	enum { result = n%2==0 ? n+1:n };
};

/**
 * IntSqrt and intSqrt both calculate the floored integer sqrt of a number,
 * either at compile or runtime.
 */
template<class T>
T intSqrt(T n)
{
	T a;
	for (a=0;n>=(((T)2)*a)+((T)1);n-=(((T)2)*a++)+((T)1));
	return a;
}

template<uint64_t n, uint64_t lo, uint64_t hi>
struct IntSqrt;

template<bool cond,uint64_t n, uint64_t lo, uint64_t hi, uint64_t mid>
struct IntSqrtRec {
	enum { result = IntSqrt<n,lo,mid-1>::result };
};

template<uint64_t n, uint64_t lo, uint64_t hi,uint64_t mid>
struct IntSqrtRec<false,n,lo,hi,mid> {
	enum { result = IntSqrt<n,mid,hi>::result };
};

template<uint64_t n, uint64_t lo=1, uint64_t hi=n>
struct IntSqrt {
	enum { mid = (lo+hi+1)/2 };
	enum { result = IntSqrtRec<n < (uint64_t)mid*(uint64_t)mid ,n,lo,hi,mid>::result };
};

template<uint64_t n, uint64_t m>
struct IntSqrt<n,m,m> {
	enum { result = m};
};

#endif /* SQRT_HPP_ */
