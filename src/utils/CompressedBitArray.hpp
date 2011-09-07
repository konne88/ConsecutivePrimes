#ifndef COMPRESSED_BIT_ARRAY_H_
#define COMPRESSED_BIT_ARRAY_H_

#include "stdio.h"
#include <stdio.h>
#include <string.h>
#include <new>
#include <assert.h>

// define NDEBUG to disable assertions

/* This class represents a check list (bit array) for
 * each odd number greater or equal then a certain offset.
 *
 * Thus the k'th bit represents the number
 * number p = 2k+offset
 *
 *
 * For checklist is further separated into Sectors.
 *
 *
 *
 * Cache
 * Register
 *
 *
 *
 * But one can't access individual bits.
 *
 *
 * b is the number of bits per calcblock
 * c the number of calcblocks per cache
 * the number p is at the index k
 * p = 2k+3
 * the index k is at cache z in calcblock x and bit y
 * cbz+bx+y = k
 * p = 2(cbz+bx+y)+3
 *
// to calc the size of the numberarray we need to make sure
// that the number n (passed to function) is in the array
// n = 2(cbz+bx+y)+3
// (n-3)/2 = cbz+bx+y
//
// the size in bits s we need is defined as
// s = ((n-3)/2)+1 = (n-1)/2
// because the size is the highest index +1
//
// the amount of calcblocks l we need is
// l = (n-1)/(2b)
//
// so to use the full memory power pass numbers n that
// fullfill the following equation, where l is an int
// n = 2*b*c*l+1
 *
 * offset defines the first valid number in the array
 *
 * if offset is 3, the first bit represents the number 3
 */


template<class OffsetType, class Section, uint64_t sections, class Hardware, uint64_t startNumber>
class CompressedBitArray {
private:
	static const uint64_t bitsPerByte = Hardware::bitsPerByte;
	static const uint64_t bytesPerSection = sizeof(Section);
	static const uint64_t bitsPerSection = bytesPerSection * bitsPerByte;
	static const uint64_t bytesPerArray = sections*bytesPerSection;
	static const uint64_t bitsPerArray = sections*bitsPerSection;
	static const uint64_t numbersPerArray = bitsPerArray*2;

	OffsetType offset;
	Section* array;

	template<class T> static bool isOdd(T v){
		return (v % 2) == 1;
	}

public:
	typedef typename Hardware::UnsignedRegisterType Index;
/*
	static bool isAligned(OffsetType number) {
		assert(isOdd(number));

		return (number-firstNumber) % alignment;
	}

	static void alignedUpperBound(OffsetType upperBound, OffsetType& newUpperBound, OffsetType& arrays) {
		if(!isOdd(upperBound)) {
			newUpperBound += 1;
		}

		if(!isAligned(newUpperBound)){
			newUpperBound = (((newUpperBound-firstNumber)/alignment)+1) * alignment;
		}

		arrays = (newUpperBound-firstNumber)/alignment;

		assert(isAligned(newUpperBound));
		assert((arrays*alignment)+firstNumber == newUpperBound);
	}
*/
	OffsetType firstNumber() const {
		return offset;
	}

	OffsetType lastNumber() const {
		return offset + numbersPerArray - 2;
	}

	CompressedBitArray()
	: offset(startNumber)
	{
		assert(isOdd(offset));

		if(posix_memalign((void**)&array,getpagesize(),bytesPerArray) != 0){
			throw std::bad_alloc();
		}

		memset(array,0,bytesPerArray);
	}

	~CompressedBitArray()
	{
		free(array);
	}

	void incrementOffset() {
		memset(array,0,bytesPerArray);
		offset += numbersPerArray;
	}

	void incrementOffset(OffsetType times) {
		memset(array,0,bytesPerArray);
		offset += numbersPerArray*times;
	}

	/**
	 * calculate how many compressed bit arrays we need in order to store all numbers x
	 * startNumber <= x <= number
	 */
	static OffsetType mapNumberToArray(OffsetType number) {
		OffsetType arrays = (number-(startNumber-2)+numbersPerArray-1)/numbersPerArray;

		// checked for correctness
		assert(arrays*numbersPerArray+(startNumber-2) >= number);
		assert((arrays-1)*numbersPerArray+(startNumber-2) < number);

		return arrays;
	}

	Section& getSection(Section i) const {
		return array[i];
	}

	void mapNumber(OffsetType number, Index& section, Index& sectionBit) const {
		assert(isOdd(number));
		assert(number >= firstNumber());
		assert(number <= lastNumber());

		// TODO add ldiv_t
//		div_t r = div((number-offset)/2,bitsPerSection);
//		section = r.quot;
//		sectionBit = r.rem;
		Index pos = (number-offset)/2;

		section = pos/bitsPerSection;
		sectionBit = pos%bitsPerSection;

		// can't use unmap Number, cause that would create a loop
		assert(offset + (section*bitsPerSection + sectionBit)*2 == number);
	}

	OffsetType unmapNumber(Index section, Index sectionBit) const {
		OffsetType num = offset + (section*bitsPerSection + sectionBit)*2;

		Index section2;
		Index sectionBit2;
		mapNumber(num,section2,sectionBit2);
		assert(section2 == section);
		assert(sectionBit2 == sectionBit);
		return num;
	}

	void setBit(OffsetType number, bool value) {
		Index section;
		Index sectionBit;
		mapNumber(number,section,sectionBit);
		setBit(section,sectionBit,value);
	}

	void setBit(Index section, Index sectionBit, bool value) {
		getSection(section) |= (((Section)1)<<sectionBit);
		assert(testBit(section,sectionBit) == value);
	}

	bool testBit(Index section, Index sectionBit) const {
		return getSection(section) & (((Section)1)<<sectionBit);
	}
};

#endif
