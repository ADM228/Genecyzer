#ifndef __RIFF_INCLUDED__
#define __RIFF_INCLUDED__

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <ios>
#include <vector>
#include <fstream>

#include "BitConverter.cpp"

#define CHUNK_NAME_LEN 4
#define CHUNK_NAME_LEN_INCHAR 5

namespace RIFF {

class RIFFChunkBase {
    public:
        virtual std::vector<uint8_t> * exportData ();
        virtual void exportData (uint8_t * outPtr);
        virtual size_t size ();
    protected:
        char type[CHUNK_NAME_LEN_INCHAR];
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

class RIFFFile : public RIFFChunkBase, protected std::fstream {      // the entire RIFF file format is a RIFF chunk in itself
    public:
        inline void open( const char* filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out );
        inline void open( const std::string& filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out );
        inline void open( const std::filesystem::path& filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out );

        using std::fstream::close;
        using std::fstream::is_open;
        ~RIFFFile();

        // Writes latest chunk to the file if open
        void writeLatestChunk();

        // Writes the entire file to the file if open
        void writeFile();
    private:
        char type[CHUNK_NAME_LEN_INCHAR] = "RIFF";
        std::vector<RIFFChunkBase> chunks;
};
RIFFFile::~RIFFFile() {
    if (std::basic_fstream<char>::is_open()) std::fstream::close();
}

void RIFFFile::writeLatestChunk() {
    auto temp = new uint8_t[chunks.back().size()];
    chunks.back().exportData(temp);
    std::fstream::write((char *)temp, chunks.back().size());
    delete[] temp;
}

void RIFFFile::open( const char* filename, std::ios_base::openmode mode) { std::fstream::open(filename, mode | std::ios::binary); }
void RIFFFile::open( const std::string& filename, std::ios_base::openmode mode) { std::fstream::open(filename, mode | std::ios::binary); }
void RIFFFile::open( const std::filesystem::path& filename, std::ios_base::openmode mode) { std::fstream::open(filename, mode | std::ios::binary); }


}

#endif  // __RIFF_INCLUDED__