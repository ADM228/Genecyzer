#ifndef __STRCONVERT_INCLUDED_
#define __STRCOVNERT_INCLUDED_

#include <string>

/* Covnerts UTF32 string to standard string */
std::string To_UTF8(const std::u32string &s);

/* Converts standard string to UTF32 string */
std::u32string To_UTF32(const std::string &s);

#endif