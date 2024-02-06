#include "RIFF.hpp"

namespace RIFF {

#ifndef __RIFF_CPP_INCLUDED__
#define __RIFF_CPP_INCLUDED__

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


#endif  // __RIFF_CPP_INCLUDED__