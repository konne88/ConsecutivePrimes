#ifndef PRIMES_SIMPLE_H_
#define PRIMES_SIMPLE_H_

#include "PrimesDirectAccessArray.hpp"

template<class PrimeType, class ArrayType>
class PrimesSimple : public PrimesDirectAccessArray<PrimeType, ArrayType> {
public:
	typename ConsecutivePrimes<PrimeType>::PrimeIter* getPrimes(PrimeType upperBound) {
		PrimeType a;
		PrimeType b;
		PrimeType size = 0;

		ArrayType* prime = getPrimeArray(upperBound);

		// init the first couple of primes
		prime[0] = 1;
		prime[1] = 1;

		// for each number >= 2
		for(a=2 ; a<=upperBound ; ++a){
			// it is a prime number
			if(prime[a] == 0){
				// cross out all multiples
				// of the prime number
				for(b=2*a; b<=upperBound ; b+=a){
					prime[b] = 1;
				}
				++size;
			}
		}

		return new typename PrimesDirectAccessArray<PrimeType, ArrayType>::PrimesByteArrayIter(prime,upperBound,size);
	}
};

#endif
