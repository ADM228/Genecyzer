#ifndef __RIFF_WRAPPER_INCLUDED__
#define __RIFF_WRAPPER_INCLUDED__

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
        inline size_t readInChunk (void *to, size_t size) {return riff_readInChunk(rr, to, size);};
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
        void close(char * filetype);

        /**
         * @brief Write in current chunk
         * @note Returns RIFF_ERROR_EOC if end of chunk is reached
         * 
         * @param to Buffer to read into
         * @param size Amount of data to read
         * @return size_t Amount of data read successfully
         */
        inline size_t writeInChunk (void *from, size_t size) {return riff_writeInChunk(rw, from, size);};
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
        void writeNewChunk (std::vector<uint8_t> & data, char * id);
        void writeNewChunk (void * data, size_t size, char * id);

        /**
         * @brief Start a new chunk after the previous one
         * @note Assumes that the position is in free space after the previous chunk, will check that
         *
         * @param id The Chunk ID
         * @return Error code 
         */
        inline int newChunk() {return riff_writerNewChunk(rw);};
        int newChunk(char * id);
        /**
         * @brief Finish writing a chunk - writes size, ID, and seeks to the first byte after this chunk
         * @note Since this is what actually writes the ID, it overrides the ID set in newChunk if it was set there
         * 
         * @param id The Chunk ID
         * @return int 
         */
        inline int finishChunk() {return riff_writerFinishChunk(rw);};
        int finishChunk(char * id);

        /**
         * @brief Start a new LIST chunk and a sublevel
         * 
         * @param type The list type
         * @return int 
         */
        inline int newListChunk() {return riff_writerNewListChunk(rw);};
        int newListChunk(char * type);

        /**
         * @brief Finish the LIST chunk and step back from the sublevel
         * 
         * @param type The list type
         * @return int 
         */
        inline int finishListChunk() {return riff_writerFinishListChunk(rw);};
        int finishListChunk(char * type);

        /**
         * @brief Set chunk ID
         * @note Overriden by the chunk ID in the finishChunk method, if set
         * 
         * @param id The chunk ID
         */
        inline void setChunkID(char * id) {memcpy (rw->c_id, id, 4);};
        /**
         * @brief Set list type
         * @note Overriden by the list type in the finishChunk method, if set
         * 
         * @param id The list type
         */
        inline void setListType(char * type) {memcpy (rw->h_type, type, 4);};
        inline void setFileType(char * type) {setListType(type);};

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

#pragma region condes

RIFFReader::RIFFReader() {
    rr = riff_readerAllocate();
    #if !RIFF_PRINT_ERRORS
        rr->fp_printf = NULL;
    #endif
}

RIFFReader::~RIFFReader() {
    riff_readerFree(rr);
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
    return riff_reader_open_file(rr, (std::FILE *)file, __size);
}

int RIFFReader::open (std::FILE & __file, size_t __size) {
    file = &__file;
    type = C_FILE|MANUAL;
    return riff_reader_open_file(rr, &__file, __size);
}

#pragma endregion

#pragma region open_mem 

int RIFFReader::open (void * __mem_ptr, size_t __size) {
    file = nullptr;
    type = MEM_PTR;
    return riff_reader_open_mem(rr, __mem_ptr, __size);
}

#pragma endregion 

#pragma region generic_ifstream_functions

size_t read_ifstream(riff_reader *rr, void *ptr, size_t size){
    auto stream = ((std::ifstream *)rr->fh);
    size_t oldg = stream->tellg();
    stream->read((char *)ptr, size);
    size_t newg = stream->tellg();
    return newg-oldg;
}

size_t seek_ifstream(riff_reader *rr, size_t pos){
    auto stream = ((std::ifstream *)rr->fh);
    stream->seekg(pos);
	return stream->tellg();
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
    type = FSTREAM;
    file = new std::ifstream;
}

int RIFFReader::open(std::ifstream & __file, size_t __size){
    type = FSTREAM|MANUAL;
    file = &__file;
    return openIfstreamCommon();
}

int RIFFReader::openIfstreamCommon(){
    auto stream = (std::ifstream*)file;
    // My own open function lmfao
    if(rr == NULL)
		return RIFF_ERROR_INVALID_HANDLE;
	rr->fh = file;
	rr->size = 0;
	rr->pos_start = stream->tellg(); //current file offset of stream considered as start of RIFF file
	
	rr->fp_read = &read_ifstream;
	rr->fp_seek = &seek_ifstream;
	
	return riff_readHeader(rr);
}

#pragma endregion

void RIFFWriter::close(char * filetype) {
    setFileType(filetype);
    close();
}

void RIFFReader::close () {
    if (!(type & MANUAL)) { // Must be automatically allocated to close
        if (type == C_FILE) {
            std::fclose((std::FILE *)file);
            free (file);
        } else if (type == FSTREAM) {
            ((std::ifstream *)file)->close();
            free (file);
        }
    }
    type = CLOSED;
}

std::string RIFFReader::errorToString (int errorCode) {
    std::string errorString(riff_reader_errorToString(errorCode));
    char buffer[2+2+(2*sizeof(size_t))+1];
    std::snprintf(buffer, 19, "[0x%zX]", rr->pos);
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
    if (rr->c_size == 0) {
        return new std::vector<uint8_t>(0);
    }
    auto outVec = new std::vector<uint8_t>(rr->c_size);
    size_t totalSize = 0, succSize;
    do {
        succSize = readInChunk(outVec->data()+totalSize, rr->c_size);
        totalSize += succSize;
    } while (succSize != 0);
#if RIFF_PRINT_ERRORS
    if (totalSize != rr->c_size && rr->fp_printf) {
        rr->fp_printf("Couldn't read the entire chunk for some reason. Successfully read %zu bytes out of %zu\n", totalSize, rr->c_size);
    } 
#endif
    return outVec;
}

#pragma region Writing

#ifdef RIFF_WRITE

#pragma region condes
RIFFWriter::RIFFWriter() {
    rw = riff_writerAllocate();
    #if !RIFF_PRINT_ERRORS
        rr->fp_printf = NULL;
    #endif
}

RIFFWriter::~RIFFWriter() {
    riff_writerFree(rw);
    close();
}
#pragma endregion

#pragma region open_C_file

int RIFFWriter::open (const char* __filename, const char * __mode) {
    auto buffer = std::string(__mode);
    {
        bool hasB = 0;
        for (auto &i : buffer) {if (i == 0x62) hasB = 1;}
        if (!hasB) buffer+="b";
    }
    file = std::fopen(buffer.c_str(), __mode);
    type = C_FILE;
    return riff_writer_open_file(rw, (std::FILE *)file);
}

int RIFFWriter::open (std::FILE & __file) {
    file = &__file;
    type = C_FILE|MANUAL;
    return riff_writer_open_file(rw, &__file);
}

#pragma endregion

#pragma region open_mem 

int RIFFWriter::openMem () {
    file = nullptr;
    type = MEM_PTR;
    return riff_writer_open_mem(rw);
}

#pragma endregion 

#pragma region generic_ofstream_functions

size_t seek_ofstream(riff_writer *rr, size_t pos){
    auto stream = ((std::ofstream *)rr->fh);
    stream->seekp(pos);
	return stream->tellp();
}

size_t write_ofstream(riff_writer *rr, void *ptr, size_t size){
    auto stream = ((std::ofstream *)rr->fh);
    size_t oldp = stream->tellp();
    stream->write((char *)ptr, size);
    size_t newp = stream->tellp();
    return newp-oldp;
}

#pragma endregion

#pragma region RIFFWriter_ofstream_methods

int RIFFWriter::open(const char * __filename, std::ios_base::openmode __mode) {
    // Set type
    setAutomaticOfstream();
    ((std::fstream*)file)->open(__filename, __mode|std::ios_base::binary);
    return openOfstreamCommon();
}

int RIFFWriter::open(const std::string & __filename, std::ios_base::openmode __mode) {
    // Set type
    setAutomaticOfstream();
    ((std::fstream*)file)->open(__filename, __mode|std::ios_base::binary);
    return openOfstreamCommon();
}

int RIFFWriter::open(const std::filesystem::path & __filename, std::ios_base::openmode __mode) {
    // Set type
    setAutomaticOfstream();
    ((std::fstream*)file)->open(__filename, __mode|std::ios_base::binary);
    return openOfstreamCommon();
}

void RIFFWriter::setAutomaticOfstream(){
    type = FSTREAM;
    file = new std::fstream;
}

int RIFFWriter::open(std::ofstream & __file){
    type = FSTREAM|MANUAL;
    file = &__file;
    return openOfstreamCommon();
}

int RIFFWriter::openOfstreamCommon(){
    auto stream = (std::ofstream*)file;
    // My own open function lmfao
    if(rw == NULL)
		return RIFF_ERROR_INVALID_HANDLE;
    rw->fh = file;
	rw->data_size = RIFF_HEADER_SIZE;
	rw->pos_start = stream->tellp(); //current file offset of stream considered as start of RIFF file
	
	rw->fp_write = &write_ofstream;
	// This conversion works due to the needed variables being stored
	// At the same offsets in both the reader and writer
	rw->fp_seek = &seek_ofstream;
	rw->fp_read = &read_ifstream;

	// we are not writing header right now
	rw->pos = rw->fp_seek(rw, RIFF_HEADER_SIZE+rw->pos_start);
	
	
	return RIFF_ERROR_NONE;
}

void RIFFWriter::closeOfstream() {
	if (rw == NULL)
		return;

	int errCode = riff_writeHeader(rw); 
	if (errCode) {
		if (rw->fp_printf)
			rw->fp_printf(riff_writer_errorToString(errCode));
	}

	return;
}

#pragma endregion

void RIFFWriter::close () {
    if (!(type & MANUAL)) { // Must be automatically allocated to close and free ptr
        if (type == C_FILE) {
            riff_writer_close_file(rw);
            std::fclose((std::FILE *)file);
            free (file);
        } else if (type == FSTREAM) {
            closeOfstream();
            ((std::ofstream *)file)->close();
            free (file);
        } else if (type == MEM_PTR) {
            file = riff_writer_close_mem(rw);   // Must read from file afterwards
        }
    } else {
        if ((type & ~MANUAL) == C_FILE)
            riff_writer_close_file(rw);
        else if ((type & ~MANUAL) == FSTREAM)
            closeOfstream();
    }
    type = CLOSED;
}


std::string RIFFWriter::errorToString (int errorCode) {
    std::string errorString(riff_writer_errorToString(errorCode));
    char buffer[2+2+(2*sizeof(size_t))+1];
    std::snprintf(buffer, 19, "[0x%zX]", rw->pos);
    std::string outstring (buffer);
    outstring += errorString;
    return outstring;
}

#pragma region typeFunctions
int RIFFWriter::newChunk(char * id) {
    setChunkID(id);
    return newChunk();
}

int RIFFWriter::finishChunk(char * id) {
    setChunkID(id);
    return finishChunk();
}

int RIFFWriter::newListChunk(char * type) {
    int errCode = newListChunk();
    if (errCode) return errCode;
    setListType(type);
    return RIFF_ERROR_NONE;
}

int RIFFWriter::finishListChunk(char * type) {
    setListType(type);
    return finishListChunk();
}
#pragma endregion

void RIFFWriter::writeNewChunk (std::vector<uint8_t> & data, char * id){
    int errCode;
    errCode = newChunk(); if (errCode) {if (rw->fp_printf != NULL) rw->fp_printf(errorToString(errCode).c_str()); return;}
    writeInChunk(data.data(), data.size()); if (errCode) {if (rw->fp_printf != NULL) rw->fp_printf(errorToString(errCode).c_str()); return;}
    finishChunk(id); if (errCode) {if (rw->fp_printf != NULL) rw->fp_printf(errorToString(errCode).c_str());}
}

void RIFFWriter::writeNewChunk (void * data, size_t size, char * id){
    int errCode;
    errCode = newChunk(); if (errCode) {if (rw->fp_printf != NULL) rw->fp_printf(errorToString(errCode).c_str()); return;}
    writeInChunk(data, size); if (errCode) {if (rw->fp_printf != NULL) rw->fp_printf(errorToString(errCode).c_str()); return;}
    finishChunk(id); if (errCode) {if (rw->fp_printf != NULL) rw->fp_printf(errorToString(errCode).c_str());}
}

#endif

#pragma endregion

}


#endif