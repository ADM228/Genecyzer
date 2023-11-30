#ifndef __STRCONVERT_INCLUDED__
#define __STRCONVERT_INCLUDED__

#include "SFML/System/Vector2.hpp"
#include <cmath>
#include <string>
#include <locale>
#include <codecvt>
#include <vector>
#include <SFML/System.hpp>

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


template <typename T, typename U>
inline sf::Vector2<T>& operator/=(sf::Vector2<T> &left, U right) {
    left = sf::Vector2<T>(left.x/right, left.y/right);
    return left;
}

template <typename T, typename U>
inline sf::Vector2<T> operator*(sf::Vector2<T>& left, U right) {
    return sf::Vector2<T>(left.x*right, left.y*right);
}

template <typename T, typename U>
inline sf::Vector2<T> operator/(sf::Vector2<T>& left, U right) {
    return sf::Vector2f(left.x/right, left.y/right);
}

template <typename T>
sf::Vector2<T> perpendiculate(sf::Vector2<T>& vector) {
    return sf::Vector2<T>(vector.y, -vector.x);
}

void printByteArray(void * ptr, size_t size, size_t divide = 0) {
    if (divide == 0) divide = size;
    size_t repeats = (size_t)std::ceil((long double)size/(long double)divide);
    size_t idx = 0;
    for (size_t i = 0; i < repeats; i++){
        for (size_t j = 0; j < divide && idx+j < size; j++) {
            printf("%02X ", *((uint8_t *)ptr+(idx+j)));
        }
        printf("| ");
        for (size_t j = 0; j < divide && idx < size; j++){
            printf("%c", *((char *)ptr+(idx++)));
        }
        printf("\n");
        fflush(stdout);
    }
}

#endif  // __STRCONVERT_INCLUDED__