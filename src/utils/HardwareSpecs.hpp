/*
 * HardwareSpecs.h
 *
 *  Created on: Jul 11, 2011
 *      Author: konne
 */

#ifndef HARDWARESPECS_H_
#define HARDWARESPECS_H_

template<uint64_t _cacheSize, uint64_t _coreCacheSize, uint64_t _bitsPerByte = 8, class RegType = long, class URegType = unsigned long>
class HardwareSpecs {
public:
	static const uint64_t bitsPerByte = _bitsPerByte;
	static const uint64_t cacheSize = _cacheSize;
	static const uint64_t coreCacheSize = _coreCacheSize;

	typedef RegType RegisterType;
	typedef URegType UnsignedRegisterType;
};

#endif /* HARDWARESPECS_H_ */
