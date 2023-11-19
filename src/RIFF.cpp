#ifndef __RIFF_INCLUDED__
#define __RIFF_INCLUDED__

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <forward_list>
#include <ios>
#include <memory>
#include <vector>
#include <fstream>
#include <iterator>

#include "BitConverter.cpp"

#define CHUNK_NAME_LEN 4

namespace RIFF {

    enum RIFFError : int {

    //Error codes (pass to riff_errorToString()), value mapping may change in the future
    //non critical
    RIFF_ERROR_NONE = 0,    //no error
    RIFF_ERROR_EOC,         //end of current chunk, when trying to read/seek beyond end of current chunk data
    RIFF_ERROR_EOCL,        //end of chunk list, if you are already at the last chunk in the current list level, occures when trying to seek the next chunk
    RIFF_ERROR_EXDAT,       //excess bytes at end of chunk list level, not critical, the rest is simply ignored (1-7 bytes inside list, otherwise a following chunk is expected - more at file level possible), should never occur
    RIFF_ERROR_EOF,         //same as RIFF_ERROR_EOCL but on the file level

    //critical errors
    RIFF_ERROR_CRITICAL = 0x80,  //first critical error code ( & RIFF_ERROR_CRITICAL is always critical error)

    RIFF_ERROR_ILLID,       //illegal ID, ID (or type) contains not printable or non ASCII characters or is wrong
    RIFF_ERROR_ICSIZE,      //invalid chunk size value in chunk header, too small or value exceeds list level or file - indicates corruption or cut off file 
    RIFF_ERROR_FILESIZE,    //unexpected end of RIFF file, indicates corruption (wrong chunk size field) or a cut off file or the passed size parameter was wrong (too small) upon opening
    RIFF_ERROR_ACCESS,      //access error, indicating that the file is not accessible (permissions, invalid file handle, etc.)

    RIFF_ERROR_INVALID_HANDLE,  //riff_handle is not set up or is NULL
    
};

class RIFFChunkBase {
    public:
        virtual int readData (uint8_t * ptr);
        virtual std::vector<uint8_t> * exportData ();
        virtual void exportData (uint8_t * outPtr);
        virtual size_t size ();
    protected:
        char type[CHUNK_NAME_LEN+1];
        std::vector<uint8_t> data;
        inline bool invalidHeader ();
        template <typename T> static inline bool invalidHeader (T * ptr);
};

bool RIFFChunkBase::invalidHeader () {
    for (char i : type) {if (i&0x80) return 1;} return 0;
}

template <typename T> bool RIFFChunkBase::invalidHeader (T * ptr) {
    for (uint_fast8_t i = 0; i < CHUNK_NAME_LEN; i++) {if (ptr[i]&0x80) return 1;} return 0;
}

int RIFFChunkBase::readData (uint8_t * ptr) {
    std::copy_n(ptr, CHUNK_NAME_LEN, type);
    if (invalidHeader()) return RIFF_ERROR_ILLID;

    char buffer[sizeof(uint32_t)+1];
    ptr+=CHUNK_NAME_LEN;
    std::copy_n(ptr, sizeof(uint32_t), buffer);
    data = std::vector<uint8_t>(readUint32(buffer));

    ptr+=sizeof(uint32_t);
    std::copy_n(ptr, data.size(), data.data());
    
    return RIFF_ERROR_NONE;
}

std::vector<uint8_t> * RIFFChunkBase::exportData() {
    if (invalidHeader()) return nullptr;
    auto output = new std::vector<uint8_t>(size());
    exportData(output->data());
    return output;
}

void RIFFChunkBase::exportData(uint8_t * outPtr) {
    if (invalidHeader()) return;
    std::copy_n(type, CHUNK_NAME_LEN, outPtr);
    writeBytes((uint32_t)data.size(), outPtr+CHUNK_NAME_LEN);
    std::copy_n(data.data(), data.size(), outPtr+CHUNK_NAME_LEN+sizeof(uint32_t));
}

size_t RIFFChunkBase::size() {
    return data.size()+CHUNK_NAME_LEN+sizeof(uint32_t);
}

class RIFFListChunk : public RIFFChunkBase {
    public:
        int readData (uint8_t * ptr) override;
        std::vector<uint8_t> * exportData () override;
        void exportData (uint8_t * outPtr) override;
        size_t size() override;
    private:
        char subtype[CHUNK_NAME_LEN+1];
        std::vector<RIFFChunkBase> subchunks;
};

int RIFFListChunk::readData (uint8_t * ptr) {
    std::copy_n(ptr, CHUNK_NAME_LEN, type);
    if (invalidHeader()) return RIFF_ERROR_ILLID;

    char buffer[sizeof(uint32_t)+1];
    ptr+=CHUNK_NAME_LEN;
    std::copy_n(ptr, sizeof(uint32_t), buffer);
    data = std::vector<uint8_t>(readUint32(buffer-CHUNK_NAME_LEN));

    ptr+=sizeof(uint32_t);
    std::copy_n(ptr, CHUNK_NAME_LEN, subtype);
    if (invalidHeader(subtype)) return RIFF_ERROR_ILLID;

    ptr+=CHUNK_NAME_LEN;
    std::copy_n(ptr, data.size(), data.data());
    
    return RIFF_ERROR_NONE;
}

std::vector<uint8_t> * RIFFListChunk::exportData() {
    if (invalidHeader()) return nullptr;
    auto output = new std::vector<uint8_t>(size());
    exportData(output->data());
    return output;
}

void RIFFListChunk::exportData(uint8_t * outPtr) {
    if (invalidHeader()) return;
    uint8_t * ptr = outPtr;
    std::copy_n(type, CHUNK_NAME_LEN, outPtr);
    writeBytes((uint32_t)data.size(), outPtr+CHUNK_NAME_LEN);
    std::copy_n(type, CHUNK_NAME_LEN, outPtr+CHUNK_NAME_LEN+sizeof(uint32_t));
    ptr += CHUNK_NAME_LEN+sizeof(uint32_t)+CHUNK_NAME_LEN;
    for (RIFFChunkBase & subchunk : subchunks) {
        subchunk.exportData(ptr);
        ptr += subchunk.size();
    }
}

size_t RIFFListChunk::size() {
    size_t output = CHUNK_NAME_LEN+sizeof(uint32_t)+CHUNK_NAME_LEN;
    for (RIFFChunkBase & subchunk : subchunks) output += subchunk.size();
    return output;
}

struct RIFFPtr {
    RIFFChunkBase chunk;
    std::streampos ptr;
};

#define readExit(__s, __t) std::fstream::read(__s, __t); if (fail()) return RIFF_ERROR_ACCESS;

class RIFFFile : public RIFFChunkBase, protected std::fstream {      // the entire RIFF file format is a RIFF chunk in itself
    public:
        inline int open( const char* filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out );
        inline int open( const std::string& filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out );
        inline int open( const std::filesystem::path& filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out );


        using std::fstream::close;
        
        using std::fstream::is_open;
        using std::fstream::fail, std::fstream::bad, std::fstream::eof, std::fstream::good;

        ~RIFFFile();

        // Read the latest chunk in the file, returns RIFF_ERROR_EOF if no chunks exist after this
        int readNextChunk();

        // Skip chunks in the file, returns RIFF_ERROR_EOF if no chunks exist after this
        int seekChunk(int offset = 1);

        // Writes latest chunk to the file if open
        void writeLatestChunk();

        // Writes the entire file to the file if open
        void writeFile();

        char type[5];
    private:
        int readHeader();
        int parseChunkHeader();

        uint32_t length = 0;
        std::vector<RIFFPtr> chunks;
        size_t chunkPtr = 1;
};
RIFFFile::~RIFFFile() {
    if (std::basic_fstream<char>::is_open()) std::fstream::close();
}

void RIFFFile::writeLatestChunk() {
    auto temp = new uint8_t[chunks.back().chunk.size()];
    chunks.back().chunk.exportData(temp);
    std::fstream::write((char *)temp, chunks.back().chunk.size());
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
    readExit(headerBuffer, CHUNK_NAME_LEN);
    if (strncmp(headerBuffer, "RIFF", CHUNK_NAME_LEN) != 0) {
        return RIFF_ERROR_INVALID_HANDLE;
    }
    
    // Read file length, should not exceed file length
    readExit(headerBuffer, 4);
    length = readUint32(headerBuffer);
    
    std::fstream::seekg(length+CHUNK_NAME_LEN+sizeof(uint32_t)-1);
    std::fstream::read(headerBuffer, 1);
    if (fail() || length < 4) return RIFF_ERROR_EOF;    // If cannot possibly even read header

    // Read file type
    std::fstream::seekg(CHUNK_NAME_LEN+sizeof(uint32_t));
    readExit(type, 4);
    
    return RIFF_ERROR_NONE;
}

int RIFFFile::seekChunk (int offset) {
    if (offset >= 0 && chunkPtr+offset >= chunks.size()) {
        for (size_t i = chunkPtr; i < chunks.size(); i++) {
            if (i >= chunks.size()) chunks.push_back({
                RIFFChunkBase(), std::fstream::tellg()
            });
            auto errorCode = parseChunkHeader();
            if (errorCode) return errorCode;
        }
    }
    // if offset >= 1 && offset+ptr >= chunks.size: parse chunk headers
    // else: just adjust ptr
    return RIFF_ERROR_NONE;
}

int RIFFFile::parseChunkHeader() {
    readExit(type, 4);
    if (invalidHeader()) return RIFF_ERROR_ILLID;

    char buffer[sizeof(uint32_t)+1];
    readExit(buffer, 4);
    data = std::vector<uint8_t>(readUint32(buffer-CHUNK_NAME_LEN));

    return RIFF_ERROR_NONE;
}



}

#endif  // __RIFF_INCLUDED__