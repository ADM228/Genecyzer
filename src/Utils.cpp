#ifndef __STRCONVERT_INCLUDED__
#define __STRCONVERT_INCLUDED__

#include <SFML/System.hpp>

#include <cmath>
#include <cstdio>
#include <cstdarg>
#include <cstdint>

#include <string>
#include <locale>
#include <vector>

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
    template<class... Args>
    deletable_facet(Args&&... args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

/* Converts UTF32 string to standard string */
std::string To_UTF8(const std::u32string &s)
{
    std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv;
    return conv.to_bytes(s);
}

/* Converts standard string to UTF32 string */
std::u32string To_UTF32(const std::string &s)
{
    std::wstring_convert<deletable_facet<std::codecvt<char32_t, char, std::mbstate_t>>, char32_t> conv;
    return conv.from_bytes(s);
}

// Uh oh seems like it's more of a general utils file now
template<std::size_t N, class T>
constexpr std::size_t countof(T(&)[N]) { return N; }

template <class T>
std::vector<T>& operator+=(std::vector<T>& lhs, std::vector<T> l) {
    lhs.insert(lhs.end(), l.begin(), l.end());
    return lhs;
}

template <std::size_t N, class T>
std::vector<T>& operator+=(std::vector<T>& lhs, std::array<T, N> l) {
    lhs.insert(lhs.end(), l.begin(), l.end());
    return lhs;
}

void printByteArray(const void * ptr, size_t size, size_t divide = 16) {
    if (divide == 0) divide = size;
    size_t repeats = (size_t)std::ceil((long double)size/(long double)divide);
    size_t idx = 0;
    for (size_t i = 0; i < repeats; i++){
        for (size_t j = 0; j < divide; j++) {
            if (idx+j < size)
                printf("%02X ", *((uint8_t *)ptr+(idx+j)));
            else
                printf("-- ");
        }
        printf("| ");
        for (size_t j = 0; j < divide && idx < size; j++){
            if (*((char * )ptr+idx) >= 0x20 && *((char * )ptr+idx) <= 0x7F)
                printf("%c", *((char *)ptr+(idx++)));
            else 
                { printf("�"); idx++; }
        }
        printf("\n");
    }
    fflush(stdout);
}

int err(const char *format, ... ){
	va_list args;
	va_start(args, format);
	int r = vfprintf(stderr, format, args);
	va_end (args);
    fflush(stderr);
	return r;
}
#endif  // __STRCONVERT_INCLUDED__