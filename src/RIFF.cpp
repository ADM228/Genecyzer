#ifndef __RIFF_INCLUDED__
#define __RIFF_INCLUDED__

#include <algorithm>
#include <cstdint>
#include <vector>

#include "BitConverter.cpp"

#define CHUNK_NAME_LEN 4

class RIFFChunkBase {
    public:
        std::vector<uint8_t> * exportData();
    private:
        char type[CHUNK_NAME_LEN];
        std::vector<uint8_t> data;
};

std::vector<uint8_t> * RIFFChunkBase::exportData() {
    for (int i = 0; i < CHUNK_NAME_LEN; i++) {if (type[i]&0x80) return nullptr;}
    auto output = new std::vector<uint8_t>(data.size()+CHUNK_NAME_LEN+sizeof(uint32_t));
    std::copy_n(data.data(), data.size(), output->begin()+CHUNK_NAME_LEN+sizeof(uint32_t));
    writeBytes((uint32_t)data.size(), output->data()+CHUNK_NAME_LEN);
    return output;
}

class RIFFFile {
    private:
        std::vector<RIFFChunkBase> chunks;
};

#endif  // __RIFF_INCLUDED__