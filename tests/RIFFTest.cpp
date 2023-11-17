#include <filesystem>
#include <iostream>
#include "../src/RIFF.cpp"

int main () {
    RIFF::RIFFFile deezNuts;
    auto out = deezNuts.open("build/test.gzr");
    std::cout << out << std::endl;
    return out;
}