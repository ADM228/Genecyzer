#ifndef __RIFF_WRAPPER_INCLUDED__
#define __RIFF_WRAPPER_INCLUDED__

#include <cstring>
#include <iostream>
#include <iterator>
#include <streambuf>
extern "C" {
    #include <stddef.h>
    #include <stdio.h>
    #include "libriff/riff.h"
}
#include <fstream>
#include <vector>
#include <filesystem>

namespace RIFF {

// Set the define RIFF_PRINT_ERRORS to 0 to disable printing of errors.
#ifndef RIFF_PRINT_ERRORS
#define RIFF_PRINT_ERRORS 1
#endif

enum fileTypes : int {
    C_FILE      = 0,
    IFSTREAM,
    MEM_PTR     = 0x10,
    MANUAL      = 0x800000, // For manually opened files
    CLOSED      = -1
};

class RIFFReader {
    public:
        RIFFReader();
        ~RIFFReader();
        /**
         * @brief Open a RIFF file with the filename and mode provided
         * Uses C's fopen(), so the filename is implementation defined
         * @note Always forces binary mode
         * @param filename Filename in fopen()'s format
         * @param mode Modes in fopen()'s format
         * @param size The expected size of the file, leave at 0 (or don't specify) if unknown
         * @return Error code
         */
        int open(const char* __filename, const char * __mode, size_t __size = 0);
        inline int open(const std::string& __filename, const char * __mode, size_t __size = 0) 
            {return open(__filename.c_str(), __mode, __size);};
        inline int open(const std::filesystem::path& __filename, const char * __mode, size_t __size = 0)
            {return open(__filename.c_str(), __mode, __size);};

        /**
         * @brief Get RIFF data from a memory pointer
         * 
         * @param mem_ptr Pointer to the memory buffer with RIFF data
         * @param size The expected size of the data, leave at 0 (or don't specify) if unknown
         * @return Error code
         */
        int open(void * __mem_ptr, size_t __size = 0);

        /**
         * @brief Open a RIFF file with the filename and mode provided
         * Uses ifstream, and always forces binary mode
         * @param filename 
         * @param mode 
         * @return Error code
         */
        int open(const char* __filename, std::ios_base::openmode __mode = std::ios_base::in, size_t __size = 0);
        int open(const std::string& __filename, std::ios_base::openmode __mode = std::ios_base::in, size_t __size = 0);
        int open(const std::filesystem::path& __filename, std::ios_base::openmode __mode = std::ios_base::in, size_t __size = 0);

        /**
         * @brief Open a RIFF file from an existing file object
         * @note The close() function of the class will not close the file object
         * @param file The file object 
         * @param size The expected size of the file, leave blank if unknown
         * @return Error code
         */
        int open(std::FILE * __file, size_t __size = 0);
        int open(std::ifstream * __file, size_t __size = 0);

        void close();

        /**
         * @brief Read in current chunk
         * @note Returns RIFF_ERROR_EOC if end of chunk is reached
         * 
         * @param to Buffer to read into
         * @param size Amount of data to read
         * @return size_t Amount of data read successfully
         */
        inline size_t readInChunk (void *to, size_t size) {return riff_readInChunk(rh, to, size);};
        /**
         * @brief Read current chunk's data
         * @note Returns nullptr if an error occurred
         * 
         * @return std::vector<uint8_t> with the data
         */
        std::vector<uint8_t> * readChunkData ();
        /**
         * @brief Seek in current chunk
         * @note Returns RIFF_ERROR_EOC if end of chunk is reached
         * @note pos 0 is first byte after chunk size (chunk offset 8)
         * 
         * @param size Amount of data to skip
         * @return Error code
         */
        inline int seekInChunk (size_t size) {return riff_readerSeekInChunk(rh, size);};
        /**
         * @brief Seek to start of next chunk within current level
         * @note ID and size are read automatically
         *
         * @return Error code 
         */
        inline int seekNextChunk () {return riff_readerSeekNextChunk(rh);};
        /**
         * @brief Seek back to data start of current chunk
         * 
         * @return Error code 
         */
        inline int seekChunkStart () {return riff_readerSeekChunkStart (rh);};
        /**
         * @brief Seek back to very first chunk of file at level 0
         * Seek back to very first chunk of file at level 0 aka the position just after opening
         * 
         * @return Error code 
         */
        inline int rewind () {return riff_readerRewind(rh);};
        /**
         * @brief Go to start of first data byte of first chunk in current level
         * 
         * @return Error code  
         */
        inline int seekLevelStart () {return riff_readerSeekLevelStart (rh);};

        /**
         * @brief Go to sub level chunk
         * Go to sub level chunk (auto seek to start of parent chunk if not already there); "LIST" chunk typically contains a list of sub chunks
         * @return Error code  
         */
        inline int seekLevelSub () {return riff_readerSeekLevelSub(rh);};
        /**
         * @brief Step back from sub list level
         * Step back from sub list level; position doesn't change and you are still inside the data section of the parent list chunk (not at the beginning of it!)
         * Returns != RIFF_ERROR_NONE if we are at level 0 already and can't go back any further
         * @return Error code  
         */
        inline int levelParent () {return riff_readerLevelParent(rh);};
        /**
         * @brief Validate chunk level structure
         * Validate chunk level structure, seeks to the first byte of the current level, seeks from chunk header to chunk header
         * To check all sub lists you need to define a recursive function
         * File position is changed by function
         * @return Error code  
         */
        inline int levelValidate () {return riff_readerLevelValidate(rh);};

        /**
         * @brief Return string to error code
         * 
         * @param errorCode 
         * @return Error string, with position at first in hex
         */
        std::string errorToString (int errorCode);


        riff_reader * rh;
        void * file;

    private:
        int type = CLOSED;

        int openIfstreamCommon();
        void setAutomaticIfstream();
};

#pragma region condes

RIFFReader::RIFFReader() {
    rh = riff_readerAllocate();
    #if !RIFF_PRINT_ERRORS
        rh->fp_printf = NULL;
    #endif
}

RIFFReader::~RIFFReader() {
    riff_readerFree(rh);
    close();
}

#pragma endregion

#pragma region open_C_file

int RIFFReader::open (const char* __filename, const char * __mode, size_t __size) {
    auto buffer = std::string(__mode);
    {
        bool hasB = 0;
        for (auto &i : buffer) {if (i == 0x62) hasB = 1;}
        if (!hasB) buffer+="b";
    }
    file = std::fopen(buffer.c_str(), __mode);
    type = C_FILE;
    return riff_reader_open_file(rh, (std::FILE *)file, __size);
}

int RIFFReader::open (std::FILE * __file, size_t __size) {
    file = __file;
    type = C_FILE|MANUAL;
    return riff_reader_open_file(rh, __file, __size);
}

#pragma endregion

#pragma region open_mem 

int RIFFReader::open (void * __mem_ptr, size_t __size) {
    file = nullptr;
    type = MEM_PTR;
    return riff_reader_open_mem(rh, __mem_ptr, __size);
}

#pragma endregion 

#pragma region generic_ifstream_functions

size_t read_ifstream(riff_reader *rh, void *ptr, size_t size){
    auto stream = ((std::ifstream *)rh->fh);
    size_t oldg = stream->tellg();
    stream->read((char *)ptr, size);
    size_t newg = stream->tellg();
    return newg-oldg;
}

size_t seek_ifstream(riff_reader *rh, size_t pos){
    auto stream = ((std::ifstream *)rh->fh);
    stream->seekg(pos);
	return stream->tellg();
}

size_t seek_ofstream(riff_writer *rh, size_t pos){
    auto stream = ((std::ofstream *)rh->fh);
    stream->seekp(pos);
	return stream->tellp();
}

size_t write_ofstream(riff_writer *rh, void *ptr, size_t size){
    auto stream = ((std::ofstream *)rh->fh);
    size_t oldp = stream->tellp();
    stream->write((char *)ptr, size);
    size_t newp = stream->tellp();
    return newp-oldp;
}

#pragma endregion

#pragma region RIFFReader_ifstream_methods

int RIFFReader::open(const char * __filename, std::ios_base::openmode __mode, size_t __size) {
    // Set type
    setAutomaticIfstream();
    ((std::ifstream*)file)->open(__filename, __mode|std::ios_base::binary);
    return openIfstreamCommon();
}

int RIFFReader::open(const std::string & __filename, std::ios_base::openmode __mode, size_t __size) {
    // Set type
    setAutomaticIfstream();
    ((std::ifstream*)file)->open(__filename, __mode|std::ios_base::binary);
    return openIfstreamCommon();
}

int RIFFReader::open(const std::filesystem::path & __filename, std::ios_base::openmode __mode, size_t __size) {
    // Set type
    setAutomaticIfstream();
    ((std::ifstream*)file)->open(__filename, __mode|std::ios_base::binary);
    return openIfstreamCommon();
}

void RIFFReader::setAutomaticIfstream(){
    type = IFSTREAM;
    file = new std::ifstream;
}

int RIFFReader::open(std::ifstream * __file, size_t __size){
    type = IFSTREAM|MANUAL;
    file = __file;
    return openIfstreamCommon();
}

int RIFFReader::openIfstreamCommon(){
    auto stream = (std::ifstream*)file;
    // My own open function lmfao
    if(rh == NULL)
		return RIFF_ERROR_INVALID_HANDLE;
	rh->fh = file;
	rh->size = 0;
	rh->pos_start = stream->tellg(); //current file offset of stream considered as start of RIFF file
	
	rh->fp_read = &read_ifstream;
	rh->fp_seek = &seek_ifstream;
	
	return riff_readHeader(rh);
}

#pragma endregion

void RIFFReader::close () {
    if (!(type & MANUAL)) { // Must be automatically allocated to close
        if (type == C_FILE) {
            std::fclose((std::FILE *)file);
            free (file);
        } else if (type == IFSTREAM) {
            ((std::ifstream *)file)->close();
            free (file);
        }
    }
    type = CLOSED;
}

std::string RIFFReader::errorToString (int errorCode) {
    std::string errorString(riff_reader_errorToString(errorCode));
    char buffer[2+2+(2*sizeof(size_t))+1];
    std::snprintf(buffer, 19, "[0x%zX]", rh->pos);
    std::string outstring (buffer);
    outstring += errorString;
    return outstring;
}

std::vector<uint8_t> * RIFFReader::readChunkData() {
    int errCode;
    errCode = seekChunkStart(); 
    if (errCode) {
        return nullptr;
    }
    if (rh->c_size == 0) {
        return new std::vector<uint8_t>(0);
    }
    auto outVec = new std::vector<uint8_t>(rh->c_size);
    size_t totalSize = 0, succSize;
    do {
        succSize = readInChunk(outVec->data()+totalSize, rh->c_size);
        totalSize += succSize;
    } while (succSize != 0);
#if RIFF_PRINT_ERRORS
    if (totalSize != rh->c_size && rh->fp_printf) {
        rh->fp_printf("Couldn't read the entire chunk for some reason. Successfully read %zu bytes out of %zu\n", totalSize, rh->c_size);
    } 
#endif
    return outVec;
}

}

#endif