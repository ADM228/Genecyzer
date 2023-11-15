#ifndef __RIFF_INCLUDED__
#define __RIFF_INCLUDED__

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "BitConverter.cpp"

#define CHUNK_NAME_LEN 4

class RIFFChunkBase {
    public:
        virtual std::vector<uint8_t> * exportData ();
        virtual void exportData (uint8_t * outPtr);
        virtual size_t size ();
    protected:
        char type[CHUNK_NAME_LEN];
        std::vector<uint8_t> data;
};

std::vector<uint8_t> * RIFFChunkBase::exportData() {
    for (char i : type) {if (i&0x80) return nullptr;}
    auto output = new std::vector<uint8_t>(size());
    exportData(output->data());
    return output;
}

void RIFFChunkBase::exportData(uint8_t * outPtr) {
    for (char i : type) {if (i&0x80) return;}
    std::copy_n(type, CHUNK_NAME_LEN, outPtr);
    writeBytes((uint32_t)data.size(), outPtr+CHUNK_NAME_LEN);
    std::copy_n(data.data(), data.size(), outPtr+CHUNK_NAME_LEN+sizeof(uint32_t));
}

size_t RIFFChunkBase::size() {
    return data.size()+CHUNK_NAME_LEN+sizeof(uint32_t);
}

class RIFFListChunk : public RIFFChunkBase {
    public:
        std::vector<uint8_t> * exportData () override;
        void exportData (uint8_t * outPtr) override;
        size_t size() override;
    private:
        std::vector<RIFFChunkBase> subchunks;
};

std::vector<uint8_t> * RIFFListChunk::exportData() {
    for (char i : type) {if (i&0x80) return nullptr;}
    auto output = new std::vector<uint8_t>(size());
    exportData(output->data());
    return output;
}

void RIFFListChunk::exportData(uint8_t * outPtr) {
    for (char i : type) {if (i&0x80) return;}
    uint8_t * ptr = outPtr;
    for (RIFFChunkBase & subchunk : subchunks) {
        subchunk.exportData(ptr);
        ptr += subchunk.size();
    }
}

size_t RIFFListChunk::size() {
    size_t output = 0;
    for (RIFFChunkBase & subchunk : subchunks) output += subchunk.size();
    return output+CHUNK_NAME_LEN+sizeof(uint32_t);
}

class RIFFFile {
    private:
        std::vector<RIFFChunkBase> chunks;
};

#endif  // __RIFF_INCLUDED__