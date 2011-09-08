#ifndef PRIMES_SINGLE_CORE_H_
#define PRIMES_SINGLE_CORE_H_

#include "stdio.h"
#include <stdio.h>
#include <string.h>
#include <new>
#include <vector>

#include "PrimesCacheBit.hpp"

template<class PrimeType, class Hardware>
class PrimesSingleCore : public PrimesCacheBit<PrimeType,Hardware,Hardware::cacheSize/4*3> {
typedef PrimesSingleCore Self;
public:
	typename ConsecutivePrimes<PrimeType>::PrimeIter* getPrimes(PrimeType upperBound) {
		typename Self::PrimeList* primeList = Self::createPrehasePrimes();

		// wrap the primeList inside of a LinkedPrimeLists
		typename Self::LinkedPrimeLists* primeLists = new typename Self::LinkedPrimeLists(primeList);

		// if upperBound is lower than firstNumber
		// all primes will have been calculated with the first method already
		if(upperBound >= Self::firstNumber){
			// find out how many caches we need to compute
			PrimeType caches = Self::Sieve::mapNumberToArray(upperBound);
			typename Self::Sieve sieve;

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
			primeList->erase(primeList->begin());

			for(PrimeType cache=0 ; cache<caches ; ++cache){
				sieve.clearArray();
				crossOutSieve(sieve,*primeLists);
				addSievePrimesToList(sieve,*primeList);
				sieve.incrementOffset();
			}

			primeList->insert(primeList->begin(),2);
		}
		return new typename PrimesCheckList<PrimeType>::PrimesListIter(primeLists,upperBound);
	}
};

#endif
