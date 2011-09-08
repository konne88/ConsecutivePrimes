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
class PrimesCheckList : public ConsecutivePrimes<PrimeType> {
protected:
	typedef std::vector<PrimeType> PrimeList;
	class LinkedPrimeLists {
	public:
		struct Element {
			~Element() {
				delete current;
			}

			PrimeList* current;
			Element* next;
		};

		LinkedPrimeLists(PrimeList* list)
		{
			assert(list != NULL);
			first = new Element();
			first->current = list;
			first->next = NULL;
			last = first;
		}

		LinkedPrimeLists()
		{
			first = NULL;
			last = NULL;
		}

		~LinkedPrimeLists() {
			Element* ele = first;
			while(ele) {
				Element* next = ele->next;
				delete ele;
				ele = next;
			}
		}

		const Element* begin() const {
			assert(first != NULL);
			return first;
		}

		void append(PrimeList* list) {
			assert(list != NULL);

			if(first == NULL) {
				first = new Element();
				first->current = list;
				first->next = NULL;
				last = first;
			} else {
				last->next = new Element();
				last = last->next;
				last->current = list;
				last->next = NULL;
			}
		}

		/**
		 * Will dispose the passed LinkedPrimeLists lists object
		 */
		void merge(LinkedPrimeLists* lists) {
			last->next = lists->first;
			last = lists->last;
			lists->first = NULL;
			delete lists;
		}

	private:
		Element* first;
		Element* last;
	};

	class PrimesListIter : public ConsecutivePrimes<PrimeType>::PrimeIter {
	private:
		LinkedPrimeLists* primeLists;
		PrimeType upperBound;
		const typename LinkedPrimeLists::Element* element;
		typename PrimeList::const_iterator iter;

	public:
		PrimesListIter(LinkedPrimeLists* list, PrimeType uB)
		: primeLists(list), upperBound(uB), element(list->begin()), iter(element->current->begin())
		{}

		virtual ~PrimesListIter() {
			delete primeLists;
		}

	public:
		/*
		 * returns the next prime number
		 * the first invocation returns the first prime number
		 * returns 0 if no primes are left
		 */
		PrimeType next() {
			if(iter == element->current->end()) {
				if(element->next) {
					element = element->next;
					iter = element->current->begin();
				} else {
					return 0;
				}
			}

			if(*iter > upperBound) {
				return 0;
			}

			const PrimeType& prime = *iter;
			++iter;   // advance to the next prime number
			return prime;
		}

		PrimeType primes() {	// FIXME remove function from interface
			return 0;
		}
	};
};

#endif /* PRIME_H_ */
