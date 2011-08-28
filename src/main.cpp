#include <iostream>
#include <time.h>
#include <boost/cstdint.hpp>

#include "PrimeTemplates.hpp"
#include "PrimesSimple.hpp"
#include "PrimesCacheBit.hpp"
#include "HardwareSpecs.hpp"

using namespace std;

typedef uint64_t PrimeType;

#ifndef CLOCKS_PER_SEC
	#define CLOCKS_PER_SEC  1000000l
#endif

int main() {
#ifndef NDEBUG
	typedef HardwareSpecs<8> Hardware;

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

		ConsecutivePrimes<PrimeType>* cp1 = new PrimesCacheBit<PrimeType,Hardware>();
		ConsecutivePrimes<PrimeType>* cp2 = new PrimesSimple<PrimeType,char>();
		ConsecutivePrimes<PrimeType>::PrimeIter* iter1 = cp1->getPrimes(n);
		ConsecutivePrimes<PrimeType>::PrimeIter* iter2 = cp2->getPrimes(n);
		PrimeType prime1 = 0;
		PrimeType prime2 = 0;
		do {
			prime1 = iter1->next();
			prime2 = iter2->next();
			assert(prime1 == prime2);
		} while(prime1 != 0);

		delete cp1;
		delete cp2;
		delete iter1;
		delete iter2;
	}

#endif



	typedef HardwareSpecs<1024*(1024+512)> RealHardware;

	clock_t start = clock();

	ConsecutivePrimes<PrimeType>* cp = new PrimesCacheBit<PrimeType,RealHardware>();
	ConsecutivePrimes<PrimeType>::PrimeIter* iter = cp->getPrimes(100000001*10);

	std::cerr << "calc done in " << ((double)clock()-start)/CLOCKS_PER_SEC << "s\n";

	PrimeType prime = 0;
	uint64_t i = -1;
	do {
		prime = iter->next();
		++i;
	} while(prime != 0);

	std::cerr << "primes: " << i << '\n';

	std::cerr << "TERMINATE";

	delete cp;
	delete iter;

	return 0;
}
