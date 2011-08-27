#ifndef CONSECUTIVE_PRIMES_H_
#define CONSECUTIVE_PRIMES_H_

/*
 * @author Konstantin Weitz
 *
 * Describes the model of a limited amount of
 * consecutive prime numbers up to an upperBound (inclusive).
 * In order to generate the prime numbers, call
 * the getPrimes member and pass the upperLimit
 * for the primes to be generated.
 *
 * After the call finishes, you can iterate
 * over the generated prime numbers using the
 * returned PrimeIter iterator's next function.
 *
 * This model is not the nicest. It would be better
 * if prime calculations would be done automatically on
 * the PrimeIter's next function invocation.
 *
 * It would also be nice to fill the model with a number
 * of already calculated primes to pick up at an earlier
 * stage.
 *
 * It was chosen to not support those things because,
 * the primary goal of this model is to show the
 * implementation of a fast consecutive prime number
 * calculation and more sophisticated models would have
 * added a lot of complexity irrelevant to this goal.
 */
template<class PrimeType>
class ConsecutivePrimes {
public:
	class PrimeIter {
	public:
		virtual ~PrimeIter() {}
		/*
		 * returns the next prime number
		 * the first invocation returns the first prime number
		 * returns 0 if no primes are left
		 */
		virtual PrimeType next() = 0;
		/*
		 * returns the number of primes
		 * this iterator will return
		 */
		virtual PrimeType primes() = 0;
	};

	/*
	 * Returns an iterator for the found prime numbers
	 * It is your job to delete it
	 */
	virtual PrimeIter* getPrimes(PrimeType upperBound) = 0;
};

#endif /* PRIME_H_ */
