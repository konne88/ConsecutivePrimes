#ifndef PRIMES_FAST_H_
#define PRIMES_FAST_H_

#include "stdio.h"
#include <stdio.h>
#include <string.h>
#include <new>
#include <vector>

#include "../utils/Math.hpp"
#include "../utils/CompressedBitArray.hpp"
#include "PrimesCheckList.hpp"

typedef PrimesCheckList<int> test;

/*
 * This algorithm finds consecutive primes using the algorithm
 * http://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
 *
 * In order to be fast on a computer, it is a good thing to be
 * cache aware. This means that we should adhere to locality
 * of reference.
 *
 * To achieve this, the sieve algorithm is slightly modified.
 * Instead of having one big sieve, we create a small sieve
 * that fits perfectly into the cache and a list of primes that
 * we have already found.
 *
 * A non prime number must have at least one prime factor that is
 * less than the floor sqrt of the number its self. This means,
 * that a prime number p may only be the smallest prime factor of a
 * non prime number n if
 * n=>p^2
 *
 * It is enough if a number n is only crossed out once. Thus it is
 * ok if only one of n's prime factor's multiples is crossed out.
 *
 * A crossed out element in the sieve means, that this is not a
 * prime number. Because of the fact above,
 * when creating a new sieve, one doesn't need to cross out all
 * multiples of primes from the primes list. It is good enough to
 * cross out only the multiples of those primes, that are
 * less than the ceil sqrt of the highest number in the sieve.
 *
 *
 *
 *
 *
 *
 *
 * One doesn't need to cross out all primes from the prime list.
 * It is enough to only use the primes up to the ceil sqrt of the
 * highest number in the current sieve.
 *
 * This algorithm assumes, that multiples of a prime number
 * don't have to be crossed out in the sieve it was found in, because
 * all numbers that
 *
 * all the numbers that would be crossed out do not matter
 * for bigger numbers in the sieve as long
 *
 * This ho
 *
 * This is definitely not true for the first sieve. Thus it is
 * computed differently. But for all other sieves it is extremely
 * likely, that this holds true. The reason why that is, is explained
 * in more detail.
 *
 * The prime number theorem states that there are approximately
 * n/log(n) prime numbers in the range of numbers from 0 to n.
 * We only need the prime numbers up to the sqrt(n).
 *
 * n/log(n) grows much faster than sqrt(n), meaning that the
 * chances that we don't find a prime number decreases.
 *
 * Considering that a sieve is 1MB in size, this means
 * the first sieve, the last prime number found, would have to be smaller
 * 1449 = sqrt(2MB) and thus there would be no prime number between
 * 1449 and 1048576.
 *
 *
 *
 */
template<class PrimeType, class Hardware>
class PrimesFast : public PrimesCheckList<PrimeType> {
private:
	typedef typename Hardware::RegisterType Section;
	static const uint64_t bitsPerByte = Hardware::bitsPerByte;
	static const uint64_t bytesPerSection = sizeof(Section);
	static const uint64_t bitsPerSection = bytesPerSection*bitsPerByte;
	static const uint64_t bytesPerCache = 1024*512*3; // Hardware::cacheSize;
	static const uint64_t bitsPerCache = bytesPerCache * bitsPerByte;
	static const uint64_t sectionsPerCache = bytesPerCache / bytesPerSection;
	static const uint64_t numbersPerCache = bitsPerCache*2;
	static const PrimeType firstNumber = CeilToOdd<IntSqrt<numbersPerCache*2>::result>::result;

	typedef CompressedBitArray<PrimeType, Section, sectionsPerCache, Hardware,firstNumber> BitArray;
	typedef typename BitArray::Index Index;
	typedef std::vector<PrimeType> PrimeList;

public:
	typename ConsecutivePrimes<PrimeType>::PrimeIter* getPrimes(PrimeType upperBound) {
		assert(bytesPerCache >= bytesPerSection);
		assert(bytesPerCache/bytesPerSection*bytesPerSection == bytesPerCache);	// cache must be multiple of section

		BitArray sieve;
		PrimeList* primeList = new PrimeList();

		// calculate the primes in the first cache with a different method
		ConsecutivePrimes<PrimeType>* cp = new PrimesSimple<PrimeType,char>();
		typename ConsecutivePrimes<PrimeType>::PrimeIter* iter = cp->getPrimes(firstNumber-1);
		PrimeType prime;
		while((prime = iter->next()) != 0){
			primeList->push_back(prime);
		}
		delete iter;
		delete cp;

		// if upperBound is lower than firstNumber
		// all primes will have been calculated with the first method already
		if(upperBound >= firstNumber){
			// find out how many caches we need to calculate
			// checked for correctness
			PrimeType caches = (upperBound-(firstNumber-2)+numbersPerCache-1)/numbersPerCache;
			assert(caches*numbersPerCache+(firstNumber-2) >= upperBound);
			assert((caches-1)*numbersPerCache+firstNumber-2 < upperBound);

//#ifndef NDEBUG
			std::cerr
			<< "-- init --\n"
			<< "bound:        "<< upperBound  << "\n"
			<< "first number: "<< firstNumber << "\n"
			<< "caches:       "<< caches << "\n"
			<< "nums/cache:   "<< numbersPerCache << "\n"
			<< "cachesize:    "<< bytesPerCache << "\n"
			<< "sctons/cache: "<< sectionsPerCache << '\n';
//#endif
			primeList->erase(primeList->begin());

			for(PrimeType cache=0 ; cache<caches ; ++cache){
				PrimeType sqrt = intSqrt(sieve.lastNumber());

#if false
				std::cerr
				<< "-- sieve --\n"
				<< "first number: "<< sieve.firstNumber()  << "\n"
				<< "last  number: "<< sieve.lastNumber() << "\n"
				<< "sqrt lastnum: "<< sqrt << "\n";
#endif
				// cross out primes from the list
				typename PrimeList::iterator iter = primeList->begin();
				for( ; iter!=primeList->end() ; ++iter){
					// TODO for a weird reason, turning this into a reference makes the code much faster
					// the loop time goes from 75% down to 15%
					const PrimeType& prime = *iter;

					if(prime > sqrt){
						break;
					}

					// for all multiples of the prime that matter
					// TODO saw that this line takes up like 75% of the time
					// check if I get better results not using prime*prime

//					(sieve.firstNumber()+prime-1)/prime

					PrimeType pp = ((sieve.firstNumber()+prime-((PrimeType)1))/prime)*prime;
					pp += prime * (pp % 2==0);

					for(PrimeType multiple=pp
							//std::max(pp,prime*prime)
//							prime*prime
							;
						multiple<=sieve.lastNumber() ;
						multiple+=prime+prime)
					{
						if(multiple >= sieve.firstNumber()) {
							sieve.setBit(multiple,true);
						}
					}
				}

				// opcontrol --start --session-dir=.
				// opcontrol --shutdown
				// opannotate --source --output-dir=annotated --session-dir=. Prime
				// opannotate --source --assembly --session-dir=. Prime
				// opreport --session-dir=. -l Prime | less
				// valgrind --leak-check=full ./Prime
				// valgrind --tool=cachegrind ./Prime


				// INST_RETIRED_ANY_P
				// CPU_CLK_UNHALTED

				// add found numbers to the prime list
				for(Index section=0 ; section<sectionsPerCache ; ++section){
					for(Index sectionBit=0 ; sectionBit<bitsPerSection ; ++sectionBit) {
						if(sieve.testBit(section,sectionBit) == false){
							// it's a prime
							PrimeType prime = sieve.unmapNumber(section,sectionBit);
							primeList->push_back(prime);
						}
					}
				}
				sieve.incrementOffset();
			}

			primeList->insert(primeList->begin(),2);
		}
		return new typename PrimesCheckList<PrimeType>::PrimesListIter(primeList,upperBound);
	}
};

#endif
