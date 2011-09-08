#ifndef PRIMES_THREADED_H_
#define PRIMES_THREADED_H_

#include "stdio.h"
#include <stdio.h>
#include <string.h>
#include <new>
#include <vector>
#include <boost/static_assert.hpp>

#include "PrimesCacheBit.hpp"

template<class PrimeType, class Hardware>
class PrimesThreaded : public PrimesCacheBit<PrimeType,Hardware,Hardware::coreCacheSize/8*7> {
typedef PrimesThreaded Self;
public:
	typename ConsecutivePrimes<PrimeType>::PrimeIter* getPrimes(PrimeType upperBound) {
		typename Self::PrimeList* primeList = Self::createPrehasePrimes();

		// if upperBound is lower than firstNumber
		// all primes will have been calculated with the first method already
		if(upperBound >= Self::firstNumber){
			PrimeType caches = Self::Sieve::mapNumberToArray(upperBound);

//#ifndef NDEBUG
			std::cerr
			<< "-- init --\n"
			<< "bound:        "<< upperBound  << "\n"
			<< "first number: "<< Self::firstNumber << "\n"
			<< "caches:       "<< caches << "\n"
			<< "nums/cache:   "<< Self::numbersPerCache << "\n"
			<< "cachesize:    "<< Self::bytesPerCache << "\n"
			<< "sctons/cache: "<< Self::sectionsPerCache << '\n';
//#endif

			// remove the 2 because it is already crossed out implicitly
			primeList->erase(primeList->begin());

			// outer loop that iterates over all tasks
			PrimeType cache=0;
			bool done = false;
			typename Self::Sieve sieve;
			do {
				const PrimeType last = Self::lastPrephaseNumber+cache*Self::numbersPerCache;
				const PrimeType lastCurrentlyCheckableNumber = last*last;
				PrimeType calculateableCaches = Self::Sieve::mapNumberToArray(lastCurrentlyCheckableNumber);

				if(calculateableCaches >= caches) {
					calculateableCaches = caches;
					done = true;
				}

				typename Self::PrimeList* primeListCopy = new typename Self::PrimeList(*primeList);

				#pragma omp parallel for ordered schedule(dynamic) private(sieve) shared(primeList)
				for(PrimeType i=cache ; i<calculateableCaches ; ++i){
				//	std::cerr << "-------------\n" << cache << " " << calculateableCaches << " " << i << "\n";
					sieve.setArrayOffset(i);
					sieve.clearArray();
					crossOutSieve(sieve,*primeList);
					#pragma omp ordered
					addSievePrimesToList(sieve,*primeListCopy);
				}

				delete primeList;
				primeList = primeListCopy;

				cache = calculateableCaches;
			} while(!done);

#if false
				std::cerr
				<< "-- sieve --\n"
				<< "first number: "<< sieve.firstNumber()  << "\n"
				<< "last  number: "<< sieve.lastNumber() << "\n"
				<< "sqrt lastnum: "<< sqrt << "\n";
#endif

			primeList->insert(primeList->begin(),2);
		}
		return new typename PrimesCheckList<PrimeType>::PrimesListIter(primeList,upperBound);
	}
};

#endif
