/*
libriff

Author/copyright: Markus Wolf, alexmush
License: zlib (https://opensource.org/licenses/Zlib)


To read or write any RIFF files.
Not specialized to specific types like AVI or WAV.
Special chunks (e.g. "LIST") can contain a nested sub list of chunks


Example structure of RIFF file:

chunk list start ("RIFF") - ID,size,type
  type (of parent chunk header)
  chunk block - list level 0
  chunk block ("LIST") - level 0 - ID,size,type
    type (of parent chunk header)
    chunk block - level 1
    chunk block - level 1
  chunk block - level 0


Reader usage:
Use a default open-function (file, mem) or create your own
  The required function pointers for reading and seeking are set here
  When creating your own open-function, take a look at the default function code as template
After the file is opened we are in the first chunk at list level 0
 You can freely read and seek within the data of the current chunk
 Use riff_readerSeekNextChunk() to go to the first data byte of the next chunk (chunk header is read already then)
If the current chunk contains a list of sub level chunks:
 Call riff_readerSeekLevelSub() to be positioned at the first data byte of the first chunk in the sub list level
 Call riff_readerLevelParent() to leave the sub list without changing the file position
Read members of the riff_reader to get all info about current file position, current chunk, etc.

Writer usage:
Define RIFF_WRITE to make the writing functions available (this is to reduce bloat in projects that do not need writing). 
Use a default open-function (file, mem) or create your own
  The required function pointers for reading, seeking and writing are set here
  When creating your own open-function, take a look at the default function code as template
After the file is opened we are in freespace, where we can create a chunk with riff_writerNewChunk()
  After creating a chunk you can fill it with data
  Make sure to set the chunk ID via memcpy() or str(n)cpy() into c_id
  After writing, make sure to finish the chunk with riff_writerFinishChunk() to write the header
LIST Chunks can also be created with riff_writerNewListChunk(), they automatically descend you into the sublevel
  Create chunks (or LIST chunks, it can be multilevel!) within there
  Make sure to set the list type via memcpy() or str(n)cpy() into h_type
  After you're done with the sublevel, you can exit the sublevel by finishing the LIST with riff_writerFinishListChunk()
Set the file type via memcpy() or str(n)cpy() into h_type.
After you're done writing the file, *close the file* to write the file header. The object that riff_writer wrote to is not deallocated on closing, you need to do that yourself.

You can overwrite existing data, and navigate thru it; the file size will never shrink however.
When writing to memory, the pointer is allocated automatically and is kept inside; you get the pointer at the very end from calling riff_writer_close_mem(), the pointer is also accessible from fh; its size can be obtained from size. Use free() to free the memory after its use to avoid memory leaks.

May not work for RIFF files larger than 2GB.
*/




#ifndef _RIFF_H_
#define _RIFF_H_

#include <stddef.h>
#include <stdio.h>


#define RIFF_HEADER_SIZE  12      //size of RIFF file header and RIFF/LIST chunks that contain subchunks
#define RIFF_CHUNK_DATA_OFFSET 8  //offset from start of chunk, size of chunk ID + chunk size field.


//Error codes (pass to riff_errorToString()), value mapping may change in the future
//non critical
#define RIFF_ERROR_NONE   0  //no error
#define RIFF_ERROR_EOC    1  //end of current chunk, when trying to read/seek beyond end of current chunk data
#define RIFF_ERROR_EOCL   2  //end of chunk list, if you are already at the last chunk in the current list level, occures when trying to seek the next chunk
#define RIFF_ERROR_EXDAT  3  //excess bytes at end of chunk list level, not critical, the rest is simply ignored (1-7 bytes inside list, otherwise a following chunk is expected - more at file level possible), should never occur

#define RIFF_WRITER_ERROR_NOTCHUNK	1	// Not currently writin in a chunk, cannot finish
#define RIFF_WRITER_ERROR_NOTLIST	2	// Not currently writin in a chunk list, cannot finish

//critical errors
#define RIFF_ERROR_CRITICAL  4  //first critical error code ( >= RIFF_ERROR_CRITICAL is always critical error)

#define RIFF_ERROR_ILLID     4  //illegal ID, ID (or type) contains not printable or non ASCII characters or is wrong
#define RIFF_ERROR_ICSIZE    5  //invalid chunk size value in chunk header, too small or value exceeds list level or file - indicates corruption or cut off file 
#define RIFF_ERROR_EOF       6  //unexpected end of RIFF file, indicates corruption (wrong chunk size field) or a cut off file or the passed size parameter was wrong (too small) upon opening
#define RIFF_ERROR_ACCESS    7  //access error, indicating that the file is not accessible (permissions, invalid file handle, etc.)

#define RIFF_ERROR_INVALID_HANDLE 8  //riff_reader is not set up or is NULL

#define RIFF_ERROR_UNKNOWN 9


/*
//riff header
//unpacked -> do not read directly to it from file
typedef struct riff_header
{
	unsigned char chunkID[5];    //offs 0x00 (with terminator to be printable)
	int chunkDataSize;           //offs 0x04
	unsigned char riffType[5];   //offs 0x08 
} riff_header;
*/




//level stack entry
//needed to retrace from sub level (list) chunk
//header info of parent
struct riff_levelStackE {
	size_t c_pos_start;        //absolute chunk position in file stream, start of chunk header
	char c_id[5];    //ID of chunk
	size_t c_size;             //chunk size without chunk header (value as stored in RIFF file)
	char c_type[5];  //(form) type ID of chunk (available for all chunks containing sub chunks) - at level 0 it is the RIFF form type
};


//RIFF reader structure
//- Members are public and intended for read access (to avoid a plethora of get-functions)
//  Be careful with the stack, check "ls_size" first
typedef struct riff_reader {
	// ******** For internal use:
	
	
	//read bytes; required
	size_t (*fp_read)(struct riff_reader *rr, void *ptr, size_t size);
	
	//seek position relative to start pos; required
	size_t (*fp_seek)(struct riff_reader *rr, size_t pos);
	
	//print error; optional;
	//allocate function maps it to vfprintf(stderr, ...) by default; set to NULL after allocation to disable any printing
	//to be assigned before calling riff_open_...()
	int (*fp_printf)(const char * format, ... );

	// ******** For user use:

	void *fh;  //file handle or memory address, only accessed by user FP functions

	struct riff_levelStackE *ls;   //level stack, resizes dynamically, to access the parent chunk data: h->ls[h->ls_level-1]
	size_t ls_size;     //size of stack in num. elements, stack extends automatically if needed
	int ls_level;       //current level, starts at 0


	size_t c_pos_start; //start pos of current chunk (absolute pos)
	size_t c_pos;       //position in current chunk (offset in data block)
	//int c_pos_data;    //position in chunk data (c_pos + 8)
	char c_id[5];       //id of current chunk + terminator
	size_t c_size;      //size of current chunk data in bytes (value stored in file), excluding chunk header
	char pad;           //1 if c_size is odd, else 0 (indicates unused extra byte at end of chunk)

	size_t size;      //total size of RIFF file, 0 means unspecified
	size_t pos;       //current position in stream

	//RIFF file header info, available once the file is opened (could have been put)
	size_t pos_start;  //start pos of RIFF file
	char h_type[5];    //type of file FOURCC + terminator
	// * Up to this point the contents of riff_reader and riff_writer match

	size_t h_size;     //size value given in header (h_size + 8 == file_size)
	char h_id[5];      //"RIFF" + terminator

	

	
	
	
	
	
} riff_reader;

typedef struct riff_writer {
	// ******** For internal use:
		
	//read bytes; required to seek to next chunk
	size_t (*fp_read)(struct riff_reader *rr, void *ptr, size_t size);
	
	//seek position relative to start pos; required
	size_t (*fp_seek)(struct riff_writer *rw, size_t pos);
	
	//print error; optional;
	//allocate function maps it to vfprintf(stderr, ...) by default; set to NULL after allocation to disable any printing
	//to be assigned before calling riff_open_...()
	int (*fp_printf)(const char * format, ... );

	// ******** For user use:

	void *fh;  //file handle or memory address, only accessed by user FP functions

	struct riff_levelStackE *ls;   //level stack, resizes dynamically, to access the parent chunk data: h->ls[h->ls_level-1]
	size_t ls_size;     //size of stack in num. elements, stack extends automatically if needed
	int ls_level;       //current level, starts at 0

	size_t c_pos_start; //start pos of current chunk (absolute pos)
	size_t c_pos;       //position in current chunk (offset inside data block)
	//int c_pos_data;    //position in chunk data (c_pos + 8)
	char c_id[5];       //id of current chunk + terminator
	size_t c_size;      //size of current chunk data in bytes (value stored in file), excluding chunk header
	char pad;           //1 if c_size is odd, else 0 (indicates unused extra byte at end of chunk)

	size_t size;      //total size of RIFF file, 0 means unspecified
	size_t pos;       //current position in stream, relative to very start

	size_t pos_start;  //start pos of RIFF file / current LIST chunk
	char h_type[5];
	// * Up to this point the contents of riff_reader and riff_writer match

	size_t data_size;

	char state;			// For internal use

	// ******** For internal use
	//write bytes; required
	size_t (*fp_write)(struct riff_writer *rw, void *ptr, size_t size);
	

} riff_writer;




//Allocate, initialize and return reader;
riff_reader *riff_readerAllocate();

//Free allocated reader memory
void riff_readerFree(riff_reader *rr);

#ifdef RIFF_WRITE
//Allocate, initialize and return writer;
riff_writer *riff_writerAllocate();

//Free allocated writer memory
void riff_writerFree(riff_writer *rw);
#endif

//functions to parse a riff file
size_t riff_readInChunk(riff_reader *rr, void *to, size_t size); //read in current chunk, returns RIFF_ERROR_EOC if end of chunk is reached
int riff_readerSeekInChunk(riff_reader *rr, size_t c_pos);      //seek in current chunk, returns RIFF_ERROR_EOC if end of chunk is reached, pos 0 is first byte after chunk size (chunk offset 8)

#ifdef RIFF_WRITE
size_t riff_writeInChunk(riff_writer *rw, void *from, size_t size); // write in current chunk
int riff_writerSeekInChunk(riff_writer *rw, size_t c_pos);	//seek in current chunk, returns RIFF_ERROR_EOC if end of chunk is reached, pos 0 is first byte after chunk size (chunk offset 8)
#endif

int riff_readerSeekNextChunk(struct riff_reader *rr);       //seek to start of next chunk within current level, ID and size is read automatically, return
//int riff_seekNextChunkID(struct riff_reader *rr, char *id);  //find and go to next chunk with id (4 byte) in current level, fails if not found - position is invalid then -> maybe not needed, the user can do it via simple loop
int riff_readerSeekChunkStart(struct riff_reader *rr);      //seek back to data start of current chunk
int riff_readerRewind(struct riff_reader *rr);              //seek back to very first chunk of file at level 0, the position just after opening via riff_open_...()
int riff_readerSeekLevelStart(struct riff_reader *rr);      //goto start of first data byte of first chunk in current level (seek backward)
int riff_readerSeekLevelSub(struct riff_reader *rr);        //goto sub level chunk (auto seek to start of parent chunk if not already there); "LIST" chunk typically contains a list of sub chunks

#ifdef RIFF_WRITE
int riff_writerNewChunk(struct riff_writer *rw);       //reserve space for new chunk after the previous one
int riff_writerFinishChunk(struct riff_writer *rw);       //write size, id, and seek to the first byte after this chunk
int riff_writerNewListChunk(struct riff_writer *rw);       //reserve space for new list chunk after the previous one, go to sub level
//step back from sub list level; position changes to after this list chunk, just like riff_writerFinishChunk
//returns != RIFF_ERROR_NONE, if we are at level 0 already and can't go back any further
int riff_writerFinishListChunk(struct riff_writer *rw);

// writer navigation functions
int riff_writerSeekNextChunk(struct riff_writer *rw);       //seek to after this chunk, return
int riff_writerSeekChunkStart(struct riff_writer *rw);      //seek back to data start of current chunk
int riff_writerRewind(struct riff_writer *rw);              //seek back to very first chunk of file at level 0, the position just after opening via riff_open_...()
int riff_writerSeekLevelStart(struct riff_writer *rw);      //goto start of first data byte of first chunk in current level (seek backward)
#endif

//step back from sub list level; position doesn't change and you are still inside the data section of the parent list chunk (not at the beginning of it!)
//returns != RIFF_ERROR_NONE, if we are at level 0 already and can't go back any further
int riff_readerLevelParent(struct riff_reader *rr);

//int riff_seekLevelParent(struct riff_reader *rr);     //go back from sub level to the start of parent chunk (seek backward)
//int riff_seekLevelParentNext(struct riff_reader *rr); //go back from sub level to the start of the chunk following the parent chunk (seek forward)

//validate chunk level structure, seeks to the first byte of the current level, seeks from chunk header to chunk header
//to check all sub lists you need to define a recursive function
//file position is changed by function
int riff_readerLevelValidate(struct riff_reader *rr);

//return string to error code
//the current position (h->pos) tells you where in the file the problem occured
const char *riff_reader_errorToString(int e);

//return string to error code (for writer)
const char *riff_writer_errorToString(int e);


//TODO:

//validate all, follow LIST chunks
//check for duplicate chunk id in one evel
//...

//validate current level
//...




// **** User I/O init ****
// Functions used by built in open-functions
// (To be used in your user made open-function)





//read RIFF file header, return error code;
//to be called from user IO functions
int riff_readHeader(riff_reader *rr);

#ifdef RIFF_WRITE

//read RIFF file header, return error code;
//to be called from user IO functions
int riff_writeHeader(riff_writer *rw);

#endif


// **** User I/O setup ****
// Use the following built in open-functions or make your own
// Only pass a fresh allocated reader

//create and return initialized RIFF reader, FPs are set up for file access
//file position must be at the start of RIFF file, which can be nested in another file (file pos > 0)
//Since the file was opened by the user, it must be closed by the user.
//size: must be exact if > 0, pass 0 for unknown size (the correct size helps to identify file corruption)
int riff_reader_open_file(riff_reader *rr, FILE *f, size_t size);

//create and return initialized RIFF reader, FPs are set up to default for memory access
//If memory was allocated by the user, it must be deallocated by the user after use.
//size: must be > 0
int riff_reader_open_mem(riff_reader *rr, void *memptr, size_t size);


#ifdef RIFF_WRITE
//create and return initialized RIFF writer, FPs are set up for file access
//file position must be at the start of RIFF file, which can be nested in another file (file pos > 0)
//Since the file was opened by the user, it must be closed by the user.
int riff_writer_open_file(riff_writer *rw, FILE *f);

//create and return initialized RIFF writer, FPs are set up to default for memory access, memory is allocated
//size: initial size for the file
int riff_writer_open_mem(riff_writer *rw);

//finish file writing, write sizes and stuff
void riff_writer_close_file(riff_writer *rw);

//finish memory writing, write sizes, shrink to fit, return mem pointer
void * riff_writer_close_mem(riff_writer *rw);
#endif

//user open - must handle "riff_reader" allocation and setup
// e.g. for file access via network socket
// see and use "riff_open_file()" definition as template
// "int open_user(riff_reader *rr, FOO, size_t size)";




#endif // _RIFF_H_
