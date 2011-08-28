#ifndef PRIMES_LIST_H_
#define PRIMES_LIST_H_

#include <vector>

#include "ConsecutivePrimes.hpp"

/*
 * This class describes how the resulting prime numbers of
 * an algorithm are stored. In this implementation they
 * are stored in a simple array list of values of the
 * type PrimeType.
 */
template<class PrimeType>
class PrimesList : public ConsecutivePrimes<PrimeType> {
protected:
	class PrimesListIter : public ConsecutivePrimes<PrimeType>::PrimeIter {
	protected:
		typedef std::vector<PrimeType> PrimeList;

	private:
		PrimeList* primeList;
		PrimeType upperBound;
		typename PrimeList::const_iterator iter;

	public:
		PrimesListIter(PrimeList* list, PrimeType uB)
		: primeList(list), upperBound(uB), iter(list->begin())
		{}

		virtual ~PrimesListIter() {
			delete primeList;
		}

	public:
		/*
		 * returns the next prime number
		 * the first invocation returns the first prime number
		 * returns 0 if no primes are left
		 */
		PrimeType next() {
			if(iter == primeList->end() || *iter > upperBound){
				return 0;
			}

			const PrimeType& prime = *iter;
			iter++;
			// advance to the next prime number
			return prime;
		}

		PrimeType primes() {
			return primeList->size();
		}
	};
};

#endif /* PRIME_H_ */
