#ifndef PRIMES_CACHE_BIT_H_
#define PRIMES_CACHE_BIT_H_

#include "stdio.h"
#include <stdio.h>
#include <string.h>
#include <new>
#include <vector>
#include <boost/static_assert.hpp>

#include "PrimesCheckList.hpp"
#include "../utils/Math.hpp"
#include "../utils/CompressedBitArray.hpp"

/*
 * This implementation finds consecutive primes based on the algorithm
 * http://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
 *
 * To be fast on a computer, it is a good thing to be
 * cache aware. This means that we should adhere to locality
 * of reference. To achieve this, the sieve algorithm is slightly modified.
 * Instead of having one big sieve, we create a small sieve
 * that fits perfectly into the cache and a list of primes that
 * we have already found.
 *
 * Just like in the original algorithm,
 * we cross out multiples of primes. But instead of taking these
 * primes from the sieve, we take them from the list of primes.
 * After we crossed out the multiple of primes from the list,
 * we go through the sieve again and add all numbers that
 * are not crossed out to the list of primes. Then new
 * sieves are created for the numbers greater than the
 * ones of the current sieve. And the algorithm is started
 * again.
 *
 * But this approach comes with a problem. Let us assume that
 * our sieve holds the numbers x, 3<=x<=10. We have already found
 * the prime 2 and added it to the list of primes.
 * We now start to cross
 * out all the multiples of the primes from the sieve.
 * If we scan the sieve for numbers that are not crossed out,
 * we find 3, 5, 7, 9
 * This is not quite right. 9 is not a prime number. This means
 * that we have to cross out all the primes we find in the process
 * of creating the sieve too. In this case 3 would be the relevant
 * one.But there is a simpler possibility.
 *
 * The following is true for all non prime numbers n:
 * n has at least two prime factors a and b; a*b=n.
 * Because of the nature of multiplication, either
 * a or b are always <= sqrt(n). If the sqrt(n) is
 * not an integer, one of the prime factors must
 * actually be smaller than floor(sqrt(n)).
 *
 * Because it is sufficient to only cross out one of the prime factors
 * to realize that a number is not a prime,
 * it follows, that is is sufficient to only use the prime numbers p
 * p<=floor_sqrt(n) to check if the number n is a prime.
 *
 * If we apply these ideas to the problem above, we realize that the
 * primes found in the sieve, do not matter for the sieve as long as
 * they are greater than the largest number in the sieve.
 *
 * Above (or equal) which number x, can we ignore primes found in a sieve s of
 * size k, to find primes in the sieve s.
 *
 * x >= floor_sqrt(k+x)
 *
 * solve for x (we ingore floor_sqrt for now)
 *
 * x^2 >= k+x
 * x^2-x-k >= 0
 * x_12 >= (1 +- sqrt(1+4k))/2
 *
 * because a negative x doesn't make sense, we conclude
 *
 * x >= (1 + sqrt(1+4k))/2
 *
 * x will fulfill the formula above if:
 *
 * x = ((2+floor_sqrt(1+4k)) div 2) +1
 *
 * Example
 *
 * [1|2|3|4|5|6|7|8|9]
 *      x . . . . . . k=6
 * x=3
 * x+k=9
 * x = floor_sqrt(x+k)
 *
 * http://www.linuxjournal.com/article/7269
 * http://www.technovelty.org/code/arch/micro-analysis.html
 * http://openmp.org/wp/
 *
 *
 * opcontrol --init
 * opcontrol --no-vmlinux
 * opcontrol --start --session-dir=.
 * opcontrol --shutdown
 * opannotate --source --output-dir=annotated --session-dir=. Prime
 * opannotate --source --assembly --session-dir=. Prime
 * report --session-dir=. -l Prime | less
 * valgrind --leak-check=full ./Prime
 * valgrind --tool=cachegrind ./Prime

					// INST_RETIRED_ANY_P
					// CPU_CLK_UNHALTED
 *
 */
template<class PrimeType, class Hardware, int cacheSize>
class PrimesCacheBit : public PrimesCheckList<PrimeType> {
typedef PrimesCacheBit Self;
protected:
	typedef typename Hardware::RegisterType Section;
	static const uint64_t bitsPerByte = Hardware::bitsPerByte;
	static const uint64_t bytesPerSection = sizeof(Section);
	static const uint64_t bitsPerSection = bytesPerSection*bitsPerByte;
	static const uint64_t bytesPerCache = cacheSize;
	static const uint64_t bitsPerCache = bytesPerCache * bitsPerByte;
	static const uint64_t sectionsPerCache = bytesPerCache / bytesPerSection;
	static const uint64_t numbersPerCache = bitsPerCache*2;
	static const PrimeType lastPrephaseNumber = IntSqrt<numbersPerCache*2>::result; //numbersPerCache; //2+IntSqrt<1+4*numbersPerCache>::result/2+1;
	static const PrimeType firstNumber = CeilToOdd<lastPrephaseNumber+1>::result;

	static_assert(bytesPerCache >= bytesPerSection,"Cachesize must be greater than registersize.");
	static_assert(bytesPerCache/bytesPerSection*bytesPerSection == bytesPerCache,"Cachesize must be multiple of registersize.");

	static_assert(lastPrephaseNumber >= IntSqrt<firstNumber+numbersPerCache-2>::result,
			"IMPLEMENTATION ERROR: Wrong prephase upper limit");

	typedef CompressedBitArray<PrimeType, Section, sectionsPerCache, Hardware,firstNumber> Sieve;
	typedef typename Sieve::Index Index;

	static typename Self::PrimeList* createPrehasePrimes() {
		typename Self::PrimeList* primeList = new typename Self::PrimeList();

		// calculate the primes in the first cache with a different method
		ConsecutivePrimes<PrimeType>* cp = new PrimesSimple<PrimeType,char>();
		typename ConsecutivePrimes<PrimeType>::PrimeIter* iter = cp->getPrimes(lastPrephaseNumber);
		PrimeType prime;
		while((prime = iter->next()) != 0){
			primeList->push_back(prime);
		}
		delete iter;
		delete cp;

		return primeList;
	}

	static void crossOutSieve(Sieve& sieve, const typename Self::LinkedPrimeLists& primeLists) {
		PrimeType sqrt = intSqrt(sieve.lastNumber());
		const typename Self::LinkedPrimeLists::Element* ele = primeLists.begin();

		do {
			// cross out primes from the list
			typename Self::PrimeList::const_iterator iter = ele->current->begin();

			for( ; iter != ele->current->end() ; ++iter){
				const PrimeType& prime = *iter;

				if(prime > sqrt){
					return;
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
					multiple<=sieve.lastNumber();
					multiple+=prime+prime)
				{
					if(multiple >= sieve.firstNumber()) {
						sieve.setBit(multiple,true);
					}
				}
			}
		} while((ele = ele->next));
	}

	static void addSievePrimesToList(const Sieve& sieve, typename Self::PrimeList& primeList) {
		// add found numbers to the prime list
		for(Index section=0 ; section<sectionsPerCache ; ++section){
			for(Index sectionBit=0 ; sectionBit<bitsPerSection ; ++sectionBit) {
				if(sieve.testBit(section,sectionBit) == false){
					// it's a prime
					PrimeType prime = sieve.unmapNumber(section,sectionBit);
					primeList.push_back(prime);
				}
			}
		}
	}
};

#endif
