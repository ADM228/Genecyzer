#include <cstdint>
#include <vector>

// Var16 number format:

/*
Treated as 
!!! BIG-ENDIAN !!!
2 bytes if the value of the first
byte < 0x40, else it's treated as 1
byte (and 0x40 is subtracted)
*/

namespace Var16 {

#define VAR16_MIN_SIZE 1
#define VAR16_MAX_SIZE 2

#define VAR16_MIN_VALUE 0
#define VAR16_ROLLOVER_LOWBYTE 0x40

#define VAR16_MAX_VALUE ((VAR16_ROLLOVER_LOWBYTE*0x100)-1)
#define VAR16_ROLLOVER_VALUE (0x100 - VAR16_ROLLOVER_LOWBYTE)


uint16_t readBytes (uint8_t * & ptr) {
	uint16_t value; 
	if (*ptr < 0x40) {
		value = ((*ptr) << 8) | (*(ptr+1) & 0xFF);
		ptr += 2;
	} else {
		value = (*ptr) - 0x40;
		ptr++;
	}
	return value;
}

std::vector <uint8_t> encode (uint16_t value) {
	if (value < VAR16_ROLLOVER_VALUE)
		return std::vector<uint8_t> {static_cast<uint8_t>((value+0x40)&0xFF)};
	else
		return std::vector<uint8_t> {static_cast<uint8_t>(((value>>8)&0x3F)), static_cast<uint8_t>((value&0xFF))};
}

int getSize (uint16_t value) {
	return value < VAR16_ROLLOVER_VALUE ? 1 : 2;
}

template <typename T> void writeBytes (T * ptr, uint16_t value) {
	uint8_t * ptr8 = ptr;
	if (value < VAR16_ROLLOVER_VALUE)
		*ptr8 = (uint8_t)(value+0x40);
	else {
		*ptr8 = (uint8_t)((value>>8) & 0x3F);
		*(ptr8+1) = (uint8_t)(value & 0xFF);
	}
}

}	// namespace Var16