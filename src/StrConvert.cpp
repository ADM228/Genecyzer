#ifndef __STRCONVERT_INCLUDED__
#define __STRCONVERT_INCLUDED__

#include <string>
#include <locale>
#include <codecvt>

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

#endif  // __STRCONVERT_INCLUDED__