#ifndef __BITCONVERTER_INCLUDED__
#define __BITCONVERTER_INCLUDED__

#include <cstdint>
#ifdef BITCONVERTER_VECTOR_CONVS
#include <vector>
#endif
#ifdef BITCONVERTER_ARRAY_CONVS
#include <array>
#endif

namespace BitConverter {

#if !defined(__BIG_ENDIAN_OVERRIDE) && ( \
	defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
	defined(__LITTLE_ENDIAN__) || \
	defined(__ARMEL__) || \
	defined(__THUMBEL__) || \
	defined(__AARCH64EL__) || \
	defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__) )

// system is natively little-endian, no conversion needed

// Write a uint64_t to ptr, exists for portability with big-endian systems
inline void writeBytes (void * ptr, uint64_t input) { *(uint64_t *)ptr = input; }

// Write a uint32_t to ptr, exists for portability with big-endian systems
inline void writeBytes (void * ptr, uint32_t input) { *(uint32_t *)ptr = input; }

// Write a uint16_t to ptr, exists for portability with big-endian systems
inline void writeBytes (void * ptr, uint16_t input) { *(uint16_t *)ptr = input; }

// Read a uint64_t from ptr, exists for portability with big-endian systems
inline uint64_t readUint64 (const void * ptr) {return *(uint64_t *) ptr;}

// Read a uint32_t from ptr, exists for portability with big-endian systems
inline uint32_t readUint32 (const void * ptr) {return *(uint32_t *) ptr;}

// Read a uint16_t from ptr, exists for portability with big-endian systems
inline uint16_t readUint16 (const void * ptr) {return *(uint16_t *) ptr;}

#elif defined(__BIG_ENDIAN_OVERRIDE) || \
	defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
	defined(__BIG_ENDIAN__) || \
	defined(__ARMEB__) || \
	defined(__THUMBEB__) || \
	defined(__AARCH64EB__) || \
	defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__) || \
	defined(_MSC_VER)   // MSVC YOU FUCKING SON OF A BITCH

// system is natively big-endian (or undetermined in case of MSVC), conversion required

// Write a uint64_t to ptr, while converting it to little-endian
inline void writeBytes (void * ptr, uint64_t input) {
	uint8_t * tmp = (uint8_t*) ptr; 
	*(tmp  )= (input    )&0xFF;
	*(tmp+1)= (input>> 8)&0xFF;
	*(tmp+2)= (input>>16)&0xFF;
	*(tmp+3)= (input>>24)&0xFF;
	*(tmp+4)= (input>>32)&0xFF;
	*(tmp+5)= (input>>40)&0xFF;
	*(tmp+6)= (input>>48)&0xFF;
	*(tmp+7)= (input>>56)&0xFF;
}

// Write a uint32_t to ptr, while converting it to little-endian
inline void writeBytes (void * ptr, uint32_t input) {
	uint8_t * tmp = (uint8_t*) ptr; 
	*(tmp  )= (input    )&0xFF;
	*(tmp+1)= (input>> 8)&0xFF;
	*(tmp+2)= (input>>16)&0xFF;
	*(tmp+3)= (input>>24)&0xFF;
}

// Write a uint16_t to ptr, while converting it to little-endian
inline void writeBytes (void * ptr, uint16_t input) {
	uint8_t * tmp = (uint8_t*) ptr; 
	*(tmp  )= (input    )&0xFF;
	*(tmp+1)= (input>> 8)&0xFF;
}

// Read a uint64_t from ptr in little-endian
inline uint64_t readUint64 (const void * ptr) {
	uint8_t * tmp = (uint8_t*) ptr; 
	return \
		(uint64_t)*(tmp  )    |
		(uint64_t)*(tmp+1)<< 8|
		(uint64_t)*(tmp+2)<<16|
		(uint64_t)*(tmp+3)<<24|
		(uint64_t)*(tmp+4)<<32|
		(uint64_t)*(tmp+5)<<40|
		(uint64_t)*(tmp+6)<<48|
		(uint64_t)*(tmp+7)<<56;
}

// Read a uint32_t from ptr in little-endian
inline uint32_t readUint32 (const void * ptr) {
	uint8_t * tmp = (uint8_t*) ptr; 
	return \
		(uint32_t)*(tmp  )    |
		(uint32_t)*(tmp+1)<< 8|
		(uint32_t)*(tmp+2)<<16|
		(uint32_t)*(tmp+3)<<24;
}

// Read a uint16_t from ptr in little-endian
inline uint16_t readUint16 (const void * ptr) {
	uint8_t * tmp = (uint8_t*) ptr; 
	return *(tmp) | *(tmp+1)<<8;
}

#else
#error "Unsupported architecture. Please contact the lead dev and report what architecture you're compiling on"
#endif

// Architecture independent funcs

#ifdef BITCONVERTER_ARRAY_CONVS
std::array<uint8_t, 2> toByteArray(uint16_t data) {
	std::array<uint8_t, 2> output; writeBytes(output.data(), data); return output;
}

std::array<uint8_t, 4> toByteArray(uint32_t data) {
	std::array<uint8_t, 4> output; writeBytes(output.data(), data); return output;
}

std::array<uint8_t, 8> toByteArray(uint64_t data) {
	std::array<uint8_t, 8> output; writeBytes(output.data(), data); return output;
}
#endif // BITCONVERTER_ARRAY_CONVS

#ifdef BITCONVERTER_VECTOR_CONVS
std::vector<uint8_t> toVector(uint16_t data) {
	std::vector<uint8_t> output(2); writeBytes(output.data(), data); return output;
}

std::vector<uint8_t> toVector(uint32_t data) {
	std::vector<uint8_t> output(4); writeBytes(output.data(), data); return output;
}

std::vector<uint8_t> toVector(uint64_t data) {
	std::vector<uint8_t> output(8); writeBytes(output.data(), data); return output;
}
#endif // BITCONVERTER_VECTOR_CONVS

}   // namespace BitConverter

#endif  // __BITCONVERTER_INCLUDED__