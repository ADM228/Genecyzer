/*
    C++ wrapper for libriff

    (C) 2023-2024 alexmush
    License: zlib

    The C++ libriff wrapper is a memory-safe, class-based wrapper around the C-based libriff. It adds support for std::ifstream/ofstream, automatic allocation of filestreams (both C-based and if/ofstream) from filenames/filepaths, as well as some additional functions for reading/writing entire chunks at once and setting chunk types/IDs.
*/

#ifndef __RIFF_HEADER_INCLUDED__
#define __RIFF_HEADER_INCLUDED__

#include <cstring>
#include <iostream>
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
    FSTREAM,
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
        int open(const void * __mem_ptr, size_t __size = 0);

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
        int open(std::FILE & __file, size_t __size = 0);
        int open(std::ifstream & __file, size_t __size = 0);

        void close();

        /**
         * @brief Read in current chunk
         * @note Returns RIFF_ERROR_EOC if end of chunk is reached
         * 
         * @param to Buffer to read into
         * @param size Amount of data to read
         * @return size_t Amount of data read successfully
         */
        inline size_t readInChunk (void * to, size_t size) {return riff_readInChunk(rr, to, size);};
        /**
         * @brief Read current chunk's data
         * @note Returns nullptr if an error occurred
         * 
         * @return std::vector<uint8_t> with the data
         */
        std::vector<uint8_t> readChunkData ();
        /**
         * @brief Seek in current chunk
         * @note Returns RIFF_ERROR_EOC if end of chunk is reached
         * @note pos 0 is first byte after chunk size (chunk offset 8)
         * 
         * @param size Amount of data to skip
         * @return Error code
         */
        inline int seekInChunk (size_t size) {return riff_readerSeekInChunk(rr, size);};
        /**
         * @brief Seek to start of next chunk within current level
         * @note ID and size are read automatically
         *
         * @return Error code 
         */
        inline int seekNextChunk () {return riff_readerSeekNextChunk(rr);};
        /**
         * @brief Seek back to data start of current chunk
         * 
         * @return Error code 
         */
        inline int seekChunkStart () {return riff_readerSeekChunkStart (rr);};
        /**
         * @brief Seek back to very first chunk of file at level 0
         * Seek back to very first chunk of file at level 0 aka the position just after opening
         * 
         * @return Error code 
         */
        inline int rewind () {return riff_readerRewind(rr);};
        /**
         * @brief Go to start of first data byte of first chunk in current level
         * 
         * @return Error code  
         */
        inline int seekLevelStart () {return riff_readerSeekLevelStart (rr);};

        /**
         * @brief Go to sub level chunk
         * Go to sub level chunk (auto seek to start of parent chunk if not already there); "LIST" chunk typically contains a list of sub chunks
         * @return Error code  
         */
        inline int seekLevelSub () {return riff_readerSeekLevelSub(rr);};
        /**
         * @brief Step back from sub list level
         * Step back from sub list level; position doesn't change and you are still inside the data section of the parent list chunk (not at the beginning of it!)
         * Returns != RIFF_ERROR_NONE if we are at level 0 already and can't go back any further
         * @return Error code  
         */
        inline int levelParent () {return riff_readerLevelParent(rr);};
        /**
         * @brief Validate chunk level structure
         * Validate chunk level structure, seeks to the first byte of the current level, seeks from chunk header to chunk header
         * To check all sub lists you need to define a recursive function
         * File position is changed by function
         * @return Error code  
         */
        inline int levelValidate () {return riff_readerLevelValidate(rr);};

        /**
         * @brief Return string to error code
         * 
         * @param errorCode 
         * @return Error string, with position at first in hex
         */
        std::string errorToString (int errorCode);


        riff_reader * rr;
        void * file;

    private:
        int type = CLOSED;

        int openIfstreamCommon();
        void setAutomaticIfstream();
};

#ifdef RIFF_WRITE

class RIFFWriter {
    public:
        RIFFWriter();
        ~RIFFWriter();
        /**
         * @brief Open a RIFF file with the filename and mode provided
         * Uses C's fopen(), so the filename is implementation defined
         * @note Always forces binary mode
         * @param filename Filename in fopen()'s format
         * @param mode Modes in fopen()'s format
         * @return Error code
         */
        int open(const char* __filename, const char * __mode);
        inline int open(const std::string& __filename, const char * __mode) 
            {return open(__filename.c_str(), __mode);};
        inline int open(const std::filesystem::path& __filename, const char * __mode)
            {return open(__filename.c_str(), __mode);};

        /**
         * @brief Get RIFF data from a memory pointer
         * 
         * @param mem_ptr Pointer to the memory buffer with RIFF data
         * @param size The expected size of the data, leave at 0 (or don't specify) if unknown
         * @return Error code
         */
        int openMem();

        /**
         * @brief Open a RIFF file with the filename and mode provided
         * Uses ifstream, and always forces binary mode
         * @param filename 
         * @param mode 
         * @return Error code
         */
        int open(const char* __filename, std::ios_base::openmode __mode = std::ios_base::in);
        int open(const std::string& __filename, std::ios_base::openmode __mode = std::ios_base::in);
        int open(const std::filesystem::path& __filename, std::ios_base::openmode __mode = std::ios_base::in);

        /**
         * @brief Open a RIFF file from an existing file object
         * @note The close() function of the class will not close the file object
         * @param file The file object 
         * @param size The expected size of the file, leave blank if unknown
         * @return Error code
         */
        int open(std::FILE & __file);
        int open(std::ofstream & __file);

        void close();
        void close(const char * filetype);

        /**
         * @brief Write in current chunk
         * @note Returns RIFF_ERROR_EOC if end of chunk is reached
         * 
         * @param to Buffer to read into
         * @param size Amount of data to read
         * @return size_t Amount of data read successfully
         */
        inline size_t writeInChunk (const void * from, size_t size) {return riff_writeInChunk(rw, from, size);};
        /**
         * @brief Seek in current chunk
         * @note Returns RIFF_ERROR_EOC if end of chunk is reached
         * @note pos 0 is first byte after chunk size (chunk offset 8)
         * 
         * @param size Amount of data to skip
         * @return Error code
         */
        inline int seekInChunk (size_t size) {return riff_writerSeekInChunk(rw, size);};
        /**
         * @brief Write a new chunk with data
         * 
         * @param data - The data
         * @param size - Size of the data (when providing raw data from memory)
         * @param id - Chunk ID
         */
        void writeNewChunk (const std::vector<uint8_t> data, const char * id);
        void writeNewChunk (const void * data, size_t size, const char * id);

        /**
         * @brief Start a new chunk after the previous one
         * @note Assumes that the position is in free space after the previous chunk, will check that
         *
         * @param id The Chunk ID
         * @return Error code 
         */
        inline int newChunk() {return riff_writerNewChunk(rw);};
        int newChunk(const char * id);
        /**
         * @brief Finish writing a chunk - writes size, ID, and seeks to the first byte after this chunk
         * @note Since this is what actually writes the ID, it overrides the ID set in newChunk if it was set there
         * 
         * @param id The Chunk ID
         * @return int 
         */
        inline int finishChunk() {return riff_writerFinishChunk(rw);};
        int finishChunk(const char * id);

        /**
         * @brief Start a new LIST chunk and a sublevel
         * 
         * @param type The list type
         * @return int 
         */
        inline int newListChunk() {return riff_writerNewListChunk(rw);};
        int newListChunk(const char * type);

        /**
         * @brief Finish the LIST chunk and step back from the sublevel
         * 
         * @param type The list type
         * @return int 
         */
        inline int finishListChunk() {return riff_writerFinishListChunk(rw);};
        int finishListChunk(const char * type);

        /**
         * @brief Set chunk ID
         * @note Overriden by the chunk ID in the finishChunk method, if set
         * 
         * @param id The chunk ID
         */
        inline void setChunkID(const char * id) {memcpy (rw->c_id, id, 4);};
        /**
         * @brief Set list type
         * @note Overriden by the list type in the finishChunk method, if set
         * 
         * @param id The list type
         */
        inline void setListType(const char * type) {memcpy (rw->h_type, type, 4);};
        inline void setFileType(const char * type) {setListType(type);};

        /**
         * @brief Seek to start of next chunk within current level
         * @note ID and size are read automatically
         *
         * @return Error code 
         */
        inline int seekNextChunk () {return riff_writerSeekNextChunk(rw);};
        /**
         * @brief Seek back to data start of current chunk
         * 
         * @return Error code 
         */
        inline int seekChunkStart () {return riff_writerSeekChunkStart (rw);};
        /**
         * @brief Seek back to very first chunk of file at level 0
         * Seek back to very first chunk of file at level 0 aka the position just after opening
         * 
         * @return Error code 
         */
        inline int rewind () {return riff_writerRewind(rw);};
        /**
         * @brief Go to start of first data byte of first chunk in current level
         * 
         * @return Error code  
         */
        inline int seekLevelStart () {return riff_writerSeekLevelStart (rw);};


        /**
         * @brief Return string to error code
         * 
         * @param errorCode 
         * @return Error string, with position at first in hex
         */
        std::string errorToString (int errorCode);


        riff_writer * rw;
        void * file;

    private:
        int type = CLOSED;

        int openOfstreamCommon();
        void setAutomaticOfstream();
        void closeOfstream();
};

#endif  // RIFF_WRITE

}       // namespace RIFF

#endif  // __RIFF_HEADER_INCLUDED__
