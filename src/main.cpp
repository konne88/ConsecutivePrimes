#include <iostream>
#include <time.h>
#include <boost/cstdint.hpp>

#include "utils/PrimeTemplates.hpp"
#include "utils/HardwareSpecs.hpp"
#include "primes/PrimesSimple.hpp"
#include "primes/PrimesSingleCore.hpp"
#include "primes/PrimesThreaded.hpp"
#include "primes/PrimesFast.hpp"

using namespace std;

typedef uint64_t PrimeType;

#ifndef CLOCKS_PER_SEC
	#define CLOCKS_PER_SEC  1000000l
#endif

int main() {
#ifndef NDEBUG
	typedef HardwareSpecs<16> Hardware;

	assert(IntSqrt<9>::result == 3);
	assert(IntSqrt<15>::result == 3);
	assert(IntSqrt<16>::result == 4);
	assert(IntSqrt<15>::result == intSqrt(15));
	assert(IntSqrt<351>::result == intSqrt(351));
	assert(IntSqrt<128>::result == intSqrt(128));

	assert(isPrime<0>() == false);
	assert(isPrime<1>() == false);
	assert(isPrime<2>() == true);
	assert(isPrime<3>() == true);
	assert(isPrime<4>() == false);
	assert(isPrime<5>() == true);
	assert(isPrime<6>() == false);
	assert(isPrime<7>() == true);
	assert(isPrime<8>() == false);
	assert(isPrime<9>() == false);

	const uint64_t count = 25;

	int sum = 0;
	for(uint64_t i=0 ; i<count ; ++i) {
		sum += Primes<uint64_t,count>::array[i];
	}
	assert(sum == 1060);

	PrimeType nums[] = {7,9,17,22,33,100,1000,1024*512};
	//	PrimeType n = 10000; //400*1024*1024*8*2*2+1;

	for(PrimeType i=0 ; i<8 ; ++i){
		PrimeType n = nums[i];
		std::cerr << "==================== " << n << '\n';
		const int imps = 3;
		ConsecutivePrimes<PrimeType>* primes[imps];
		ConsecutivePrimes<PrimeType>::PrimeIter* iters[imps];
		primes[0] = new PrimesSingleCore<PrimeType,Hardware>();
		primes[1] = new PrimesThreaded<PrimeType,Hardware>();
		primes[2] = new PrimesSimple<PrimeType,char>();
		for(int i=0 ; i<imps ; ++i) {
			iters[i] = primes[i]->getPrimes(n);
		}

		bool continues;
		do {
			const PrimeType& prime = iters[0]->next();
			continues = prime;	// quit once all primes are 0
			for(int i=1 ; i<imps ; ++i) {
				const PrimeType p = iters[i]->next();
				assert(prime == p);
			}
		} while(continues);

		for(int i=0 ; i<imps ; ++i) {
			delete iters[i];
			delete primes[i];
		}
	}

#endif

	typedef HardwareSpecs<1024*1024*3,1024*1024*3/2> RealHardware;

	const int imps = 3;
	ConsecutivePrimes<PrimeType>* primes[imps];
	primes[0] = new PrimesSingleCore<PrimeType,RealHardware>();
	primes[1] = new PrimesThreaded<PrimeType,RealHardware>();
	primes[2] = new PrimesFast<PrimeType,RealHardware>();
	for(int s=0 ; s<2 ; ++s){
		for(int i=0 ; i<imps ; ++i) {
			std::cerr << "=================================\n";

			clock_t start = clock();
			ConsecutivePrimes<PrimeType>::PrimeIter* iter = primes[i]->getPrimes(100000001);
			std::cerr << "calc done in " << ((double)clock()-start)/CLOCKS_PER_SEC << "s\n";

			PrimeType prime = 0;
			uint64_t x = -1;
			do {
				prime = iter->next();
				++x;
			} while(prime != 0);
			std::cerr << "primes: " << x << '\n';

			delete iter;
		}
	}

	std::cerr << "TERMINATE";

	return 0;
}
