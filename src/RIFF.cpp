#ifndef __RIFF_WRAPPER_INCLUDED__
#define __RIFF_WRAPPER_INCLUDED__

extern "C" {
    #include <stddef.h>
    #include <stdio.h>
    #include "libriff/riff.h"
}
#include <fstream>
#include <filesystem>

namespace RIFF {

class RIFFFile {
    public:
        RIFFFile();
        ~RIFFFile();
        void open(const char* __filename, const char * __mode);
        inline void open(const std::string& __filename, const char * __mode) 
            {open(__filename.c_str(), __mode);};
        void open(const std::filesystem::path& __filename, const char * __mode)
            {open(__filename.c_str(), __mode);};

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
        std::FILE *file;

    private:
        bool opened;
};
RIFFFile::RIFFFile() {
    rh = riff_handleAllocate();
}

RIFFFile::~RIFFFile() {
    riff_handleFree(rh);
    std::fclose(file);
    free(file);
}

void RIFFFile::open (const char* __filename, const char * __mode) {
    file = std::fopen(__filename, __mode);
    riff_open_file(rh, file, 0);
    opened = 1;
}

void RIFFFile::close () {
    std::fclose(file);
    free (file);
    opened = 0;
}

std::string RIFFFile::errorToString (int errorCode) {
    std::string errorString(riff_errorToString(errorCode));
    char buffer[2+2+(2*sizeof(size_t))+1];
    std::snprintf(buffer, 19, "[0x%zX]", rh->pos);
    std::string outstring (buffer);
    outstring += errorString;
    return outstring;
}

}

#endif