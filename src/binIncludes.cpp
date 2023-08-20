#define INCBIN_PREFIX bin_
#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#define INCBIN_SILENCE_BITCODE_WARNING 1    // You cannot imagine how little of a fuck i give
#include "incbin.h"
INCBIN(font, "tilesetUnicode.chr");
#ifdef _MSC_VER
	#include "binIncludes.h"
#endif
