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
class PrimesThreaded : public PrimesCacheBit<PrimeType,Hardware,Hardware::coreCacheSize/4*3> {
typedef PrimesThreaded Self;
public:
	typename ConsecutivePrimes<PrimeType>::PrimeIter* getPrimes(PrimeType upperBound) {
		typename Self::PrimeList* prephasePrimeList = Self::createPrehasePrimes();	// FIXME rename to kick/jump start
		typename Self::LinkedPrimeLists* primeLists = new typename Self::LinkedPrimeLists(prephasePrimeList);

		// if upperBound is lower than firstNumber
		// all primes will have been calculated with the first method already
		if(upperBound >= Self::firstNumber){
			PrimeType caches = Self::Sieve::mapNumberToArray(upperBound);

#if LOGLEVEL >= 1
			std::cerr
			<< "-- init --\n"
			<< "bound:        "<< upperBound  << "\n"
			<< "first number: "<< Self::firstNumber << "\n"
			<< "caches:       "<< caches << "\n"
			<< "nums/cache:   "<< Self::numbersPerCache << "\n"
			<< "cachesize:    "<< Self::bytesPerCache << "\n"
			<< "sctons/cache: "<< Self::sectionsPerCache << '\n';
#endif

			// remove the 2 because it is already crossed out implicitly
			prephasePrimeList->erase(prephasePrimeList->begin());

			// outer loop that iterates over all tasks
			PrimeType cache=0;
			bool done = false;
			typename Self::Sieve sieve;
			do {
				typename Self::LinkedPrimeLists* newPrimeLists = new typename Self::LinkedPrimeLists();

				const PrimeType last = Self::lastPrephaseNumber+cache*Self::numbersPerCache;
				const PrimeType lastCurrentlyCheckableNumber = last*last;
				PrimeType calculateableCaches = Self::Sieve::mapNumberToArray(lastCurrentlyCheckableNumber);

				if(calculateableCaches >= caches) {
					calculateableCaches = caches;
					done = true;
				}

				std::cerr << "caches from " << cache << " upto " << calculateableCaches << "\n";

				#pragma omp parallel for ordered schedule(dynamic) private(sieve) shared(primeLists, newPrimeLists)
				for(PrimeType i=cache ; i<calculateableCaches ; ++i){
					typename Self::PrimeList* primeList = new typename Self::PrimeList();
					sieve.setArrayOffset(i);
					sieve.clearArray();
					crossOutSieve(sieve,*primeLists);
					addSievePrimesToList(sieve,*primeList);
					#pragma omp ordered
					newPrimeLists->append(primeList);
				}

				primeLists->merge(newPrimeLists);

				cache = calculateableCaches;
			} while(!done);

#if LOGLEVEL >= 2
      std::cerr << "-- sieve --\n"
				<< "first number: "<< sieve.firstNumber()  << "\n"
				<< "last  number: "<< sieve.lastNumber() << "\n"
				<< "sqrt lastnum: "<< sqrt << "\n";
#endif

			prephasePrimeList->insert(prephasePrimeList->begin(),2);
		}
		return new typename PrimesCheckList<PrimeType>::PrimesListIter(primeLists,upperBound);
	}
};

#endif
