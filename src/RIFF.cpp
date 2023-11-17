#ifndef __RIFF_INCLUDED__
#define __RIFF_INCLUDED__

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <ios>
#include <vector>
#include <fstream>

#include "BitConverter.cpp"

#define CHUNK_NAME_LEN 4

namespace RIFF {

    //Error codes (pass to riff_errorToString()), value mapping may change in the future
    //non critical
    #define RIFF_ERROR_NONE   0  //no error
    #define RIFF_ERROR_EOC    1  //end of current chunk, when trying to read/seek beyond end of current chunk data
    #define RIFF_ERROR_EOCL   2  //end of chunk list, if you are already at the last chunk in the current list level, occures when trying to seek the next chunk
    #define RIFF_ERROR_EXDAT  3  //excess bytes at end of chunk list level, not critical, the rest is simply ignored (1-7 bytes inside list, otherwise a following chunk is expected - more at file level possible), should never occur

    //critical errors
    #define RIFF_ERROR_CRITICAL  4  //first critical error code ( >= RIFF_ERROR_CRITICAL is always critical error)

    #define RIFF_ERROR_ILLID     4  //illegal ID, ID (or type) contains not printable or non ASCII characters or is wrong
    #define RIFF_ERROR_ICSIZE    5  //invalid chunk size value in chunk header, too small or value exceeds list level or file - indicates corruption or cut off file 
    #define RIFF_ERROR_EOF       6  //unexpected end of RIFF file, indicates corruption (wrong chunk size field) or a cut off file or the passed size parameter was wrong (too small) upon opening
    #define RIFF_ERROR_ACCESS    7  //access error, indicating that the file is not accessible (permissions, invalid file handle, etc.)

    #define RIFF_ERROR_INVALID_HANDLE 8  //riff_handle is not set up or is NULL


class RIFFChunkBase {
    public:
        virtual std::vector<uint8_t> * exportData ();
        virtual void exportData (uint8_t * outPtr);
        virtual size_t size ();
    protected:
        char type[CHUNK_NAME_LEN+1];
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
        inline int open( const char* filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out );
        inline int open( const std::string& filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out );
        inline int open( const std::filesystem::path& filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out );

        using std::fstream::close;
        using std::fstream::is_open;
        ~RIFFFile();

        // Writes latest chunk to the file if open
        void writeLatestChunk();

        // Writes the entire file to the file if open
        void writeFile();

        char type[5];
    private:
        int readHeader();

        uint32_t length;
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

int RIFFFile::open( const char* filename, std::ios_base::openmode mode) { 
    std::fstream::open(filename, mode | std::ios::binary); 
    if (fail()) return RIFF_ERROR_ACCESS; 
    return readHeader();
}

int RIFFFile::open( const std::string& filename, std::ios_base::openmode mode) { 
    std::fstream::open(filename, mode | std::ios::binary); 
    if (fail()) return RIFF_ERROR_ACCESS; 
    return readHeader(); 
}

int RIFFFile::open( const std::filesystem::path& filename, std::ios_base::openmode mode) { 
    std::fstream::open(filename, mode | std::ios::binary); 
    if (fail()) return RIFF_ERROR_ACCESS; 
    return readHeader(); 
}

template <typename T>
void printByteArray(T * ptr, size_t size) {
    for (size_t i = 0; i < size; i++) printf("%02X ", ptr[i]);
}

int RIFFFile::readHeader () {
    char headerBuffer[CHUNK_NAME_LEN+1];
    // Read first chunk, should be "RIFF"
    std::fstream::read(headerBuffer, CHUNK_NAME_LEN);
    if (fail()) return RIFF_ERROR_ACCESS; 
    if (strncmp(headerBuffer, "RIFF", CHUNK_NAME_LEN) != 0) {
        return RIFF_ERROR_ILLID;
    }
    
    // Read file length, should not exceed file length
    std::fstream::read(headerBuffer, 4);
    length = readUint32(headerBuffer);
    
    std::fstream::seekg(length+CHUNK_NAME_LEN+sizeof(uint32_t)-1);
    std::fstream::read(headerBuffer, 1);
    if (fail() || length < 4) return RIFF_ERROR_EOF;    // If cannot possibly even read header

    // Read file type
    std::fstream::seekg(CHUNK_NAME_LEN+sizeof(uint32_t));
    std::fstream::read(type, 4);

    if (fail()) return RIFF_ERROR_ACCESS;
    
    return RIFF_ERROR_NONE;
}


}

#endif  // __RIFF_INCLUDED__