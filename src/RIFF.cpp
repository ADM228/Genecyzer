#ifndef __RIFF_WRAPPER_INCLUDED__
#define __RIFF_WRAPPER_INCLUDED__

#include <cstring>
extern "C" {
    #include <stddef.h>
    #include <stdio.h>
    #include "libriff/riff.h"
}
#include <fstream>
#include <vector>
#include <filesystem>

namespace RIFF {

struct vecErr {
    int errorCode;
    std::vector<uint8_t> * data;
};

enum fileTypes : int {
    C_FILE      = 0,
    FSTREAM,
    MEM_PTR     = 0x10,
    MANUAL      = 0x800000, // For manually opened files
    CLOSED      = -1
};

class RIFFFile {
    public:
        RIFFFile();
        ~RIFFFile();
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
         * Uses fstream, and always forces binary mode
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
        int open(std::fstream * __file, size_t __size = 0);

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
         * @note Returns an error in the vecErr if an error occured
         * 
         * @return vecErr* 
         */
        vecErr * readChunkData ();
        /**
         * @brief Seek in current chunk
         * @note Returns RIFF_ERROR_EOC if end of chunk is reached
         * @note pos 0 is first byte after chunk size (chunk offset 8)
         * 
         * @param size Amount of data to skip
         * @return Error code
         */
        inline int seekInChunk (size_t size) {return riff_seekInChunk(rh, size);};
        /**
         * @brief Seek to start of next chunk within current level
         * @note ID and size are read automatically
         *
         * @return Error code 
         */
        inline int seekNextChunk () {return riff_seekNextChunk(rh);};
        /**
         * @brief Seek back to data start of current chunk
         * 
         * @return Error code 
         */
        inline int seekChunkStart () {return riff_seekChunkStart (rh);};
        /**
         * @brief Seek back to very first chunk of file at level 0
         * Seek back to very first chunk of file at level 0 aka the position just after opening
         * 
         * @return Error code 
         */
        inline int rewind () {return riff_rewind(rh);};
        /**
         * @brief Go to start of first data byte of first chunk in current level
         * 
         * @return Error code  
         */
        inline int seekLevelStart () {return riff_seekLevelStart (rh);};

        /**
         * @brief Go to sub level chunk
         * Go to sub level chunk (auto seek to start of parent chunk if not already there); "LIST" chunk typically contains a list of sub chunks
         * @return Error code  
         */
        inline int seekLevelSub () {return riff_seekLevelSub(rh);};
        /**
         * @brief Step back from sub list level
         * Step back from sub list level; position doesn't change and you are still inside the data section of the parent list chunk (not at the beginning of it!)
         * Returns != RIFF_ERROR_NONE if we are at level 0 already and can't go back any further
         * @return Error code  
         */
        inline int levelParent () {return riff_levelParent(rh);};
        /**
         * @brief Validate chunk level structure
         * Validate chunk level structure, seeks to the first byte of the current level, seeks from chunk header to chunk header
         * To check all sub lists you need to define a recursive function
         * File position is changed by function
         * @return Error code  
         */
        inline int levelValidate () {return riff_levelValidate(rh);};

        /**
         * @brief Return string to error code
         * 
         * @param errorCode 
         * @return Error string, with position at first in hex
         */
        std::string errorToString (int errorCode);


        riff_handle * rh;
        void * file;

    private:
        int type = CLOSED;

        int openFstreamCommon();
        void setAutomaticfstream();
};

#pragma region condes

RIFFFile::RIFFFile() {
    rh = riff_handleAllocate();
}

RIFFFile::~RIFFFile() {
    riff_handleFree(rh);
    close();
}

#pragma endregion

#pragma region openCfile

int RIFFFile::open (const char* __filename, const char * __mode, size_t __size) {
    auto buffer = std::string(__mode);
    {
        bool hasB = 0;
        for (auto &i : buffer) {if (i == 0x62) hasB = 1;}
        if (!hasB) buffer+="b";
    }
    file = std::fopen(buffer.c_str(), __mode);
    type = C_FILE;
    return riff_open_file(rh, (std::FILE *)file, __size);
}

int RIFFFile::open (std::FILE * __file, size_t __size) {
    file = __file;
    type = C_FILE|MANUAL;
    return riff_open_file(rh, __file, __size);
}

#pragma endregion

#pragma region openMem 

int RIFFFile::open (void * __mem_ptr, size_t __size) {
    file = nullptr;
    type = MEM_PTR;
    return riff_open_mem(rh, __mem_ptr, __size);
}

#pragma endregion 

#pragma region fstreamHandling

size_t read_fstream(riff_handle *rh, void *ptr, size_t size){
    auto stream = ((std::fstream *)rh->fh);
    size_t oldg = stream->tellg();
    stream->read((char *)ptr, size);
    size_t newg = stream->tellg();
    return newg-oldg;
}

size_t seek_fstream(riff_handle *rh, size_t pos){
    ((std::fstream *)rh->fh)->seekg(pos);
	return pos;
}

int RIFFFile::open(const char * __filename, std::ios_base::openmode __mode, size_t __size) {
    // Set type
    setAutomaticfstream();
    ((std::fstream*)file)->open(__filename, __mode|std::ios_base::binary);
    return openFstreamCommon();
}

int RIFFFile::open(const std::string & __filename, std::ios_base::openmode __mode, size_t __size) {
    // Set type
    setAutomaticfstream();
    ((std::fstream*)file)->open(__filename, __mode|std::ios_base::binary);
    return openFstreamCommon();
}

int RIFFFile::open(const std::filesystem::path & __filename, std::ios_base::openmode __mode, size_t __size) {
    // Set type
    setAutomaticfstream();
    ((std::fstream*)file)->open(__filename, __mode|std::ios_base::binary);
    return openFstreamCommon();
}

void RIFFFile::setAutomaticfstream(){
    type = FSTREAM;
    file = new std::fstream;
}

int RIFFFile::open(std::fstream * __file, size_t __size){
    type = FSTREAM|MANUAL;
    file = __file;
    return openFstreamCommon();
}

int RIFFFile::openFstreamCommon(){
    auto stream = (std::fstream*)file;
    // My own open function lmfao
    if(rh == NULL)
		return RIFF_ERROR_INVALID_HANDLE;
	rh->fh = file;
	rh->size = 0;
	rh->pos_start = stream->tellg(); //current file offset of stream considered as start of RIFF file
	
	rh->fp_read = &read_fstream;
	rh->fp_seek = &seek_fstream;
	
	return riff_readHeader(rh);
}

#pragma endregion

void RIFFFile::close () {
    if (!(type & MANUAL)) { // Must be automatically allocated to close
        if (type == C_FILE) {
            std::fclose((std::FILE *)file);
            free (file);
        } else if (type == FSTREAM) {
            ((std::fstream *)file)->close();
            free (file);
        }
    }
    type = CLOSED;
}

std::string RIFFFile::errorToString (int errorCode) {
    std::string errorString(riff_errorToString(errorCode));
    char buffer[2+2+(2*sizeof(size_t))+1];
    std::snprintf(buffer, 19, "[0x%zX]", rh->pos);
    std::string outstring (buffer);
    outstring += errorString;
    return outstring;
}

vecErr * RIFFFile::readChunkData() {
    auto outVE = new vecErr;
    int errCode;
    errCode = seekChunkStart(); 
    if (errCode) {
        outVE->errorCode = errCode;
        outVE->data = nullptr;
        return outVE;
    }
    auto outVec = new std::vector<uint8_t>(rh->c_size);
    outVE->data = outVec;
    size_t succSize = readInChunk(outVec->data(), rh->c_size);
    if (succSize == rh->c_size) {
        outVE->errorCode = 0;
        return outVE;
    } else {
        outVE->errorCode = succSize;
        return outVE;
    }
}

}

#endif