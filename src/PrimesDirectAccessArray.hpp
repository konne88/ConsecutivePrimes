#ifndef PRIMES_DIRECT_ACCESS_ARRAY_H_
#define PRIMES_DIRECT_ACCESS_ARRAY_H_

#include "ConsecutivePrimes.hpp"

/*
 * This class describes how the check list in the algorithm
 * http://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
 * is stored in memory.
 *
 * In this implementation the check list is represented by
 * an array of ArrayType values. The index to the array is also
 * the number to check for. Thus the array's element with index 10
 * represents the number 10.
 *
 * The elements in the array are accessed via direct memory access.
 * No bit shifting or such things are needed. Has the disadvantage
 * that space is wasted.
 *
 * If element in the array is 0, it is a (potential) prime number.
 * If element is set to 1, it is not a prime number.
 */
template<class PrimeType, class ArrayType>
class PrimesDirectAccessArray : public ConsecutivePrimes<PrimeType> {
protected:
	ArrayType* getPrimeArray(PrimeType upperBound) {
		return (ArrayType*)calloc(upperBound+1,sizeof(ArrayType));
	}

	class PrimesByteArrayIter : public ConsecutivePrimes<PrimeType>::PrimeIter {
	public:
		PrimesByteArrayIter(ArrayType* pA, PrimeType uB, PrimeType s)
		: size(s), index(0), primeArray(pA), upperBound(uB)
		{}
		virtual ~PrimesByteArrayIter() {
			free(primeArray);
		}

	public:
		/*
		 * returns the next prime number
		 * the first invocation return the first prime number
		 * returns 0 if no primes are left
		 */
		PrimeType next() {
			// advance to the next prime number
			do {
				++index;
				if(index > upperBound) {
					return 0;
				}
			} while(primeArray[index] == 1);

			return index;
		}

		PrimeType primes() {
			return size;
		}

	private:
		PrimeType size;
		PrimeType index;
		ArrayType* primeArray;
		PrimeType upperBound;
	};
};

#endif /* PRIME_H_ */
