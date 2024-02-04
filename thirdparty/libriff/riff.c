// take care: whenever we call rr->fp_read() or rr->fp_seek()
//   we must adjust rr->c_pos and rr->pos
//   => to simplify user wrappers we update the positions outside


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <tgmath.h>

#include <stdarg.h> //function with variable number of arguments

#include "riff.h"


#define RIFF_LEVEL_ALLOC 16  //number of stack elements allocated per step lock more when needing to enlarge (step)


//table to translate Error code to string
//shall correspond to RIFF_ERROR_... macros
static const char *riff_reader_es[] = {
	//0
	"No error",
	//1
	"End of chunk",
	//2
	"End of chunk list",
	//3
	"Excess bytes at end of file",
	
	//4
	"Illegal four character id",
	//5
	"Chunk size exceeds list level or file",
	//6
	"End of RIFF file",
	//7
	"File access failed",
	//8
	"Invalid riff_reader",
	
	
	//9
	//all other
	"Unknown RIFF error"  
};

#ifdef RIFF_WRITE

static const char *riff_writer_es[] = {
	//0
	"No error",
	//1
	"Not currently in chunk",
	//2
	"Not currently in chunk list",
	//3
	"",
	
	//4
	"Illegal four character id",
	//5
	"",
	//6
	"",
	//7
	"File access failed",
	//8
	"Invalid riff_writer",
	
	
	//9
	//all other
	"Unknown RIFF error"  
};

#define checkHandle() if (rw == NULL) return RIFF_ERROR_INVALID_HANDLE

#define RIFFWriterStateNotInChunk 0x00
#define RIFFWriterStateInChunk 0x01

#endif



//*** default access FP setup ***

/*****************************************************************************/
//default print function
int riff_printf(const char *format, ... ){
	va_list args;
	va_start(args, format);
	int r = vfprintf(stderr, format, args);
	va_end (args);
	return r;
}

/*****************************************************************************/
//seek to pos
int seekToPosR(riff_reader * rr) {
	return rr->fp_seek(rr, rr->pos);
}

int seekToPosW(riff_writer * rw) {
	return rw->fp_seek(rw, rw->pos);
}

//** FILE **


/*****************************************************************************/
size_t read_file(riff_reader *rr, void *ptr, size_t size){
	return fread(ptr, 1, size, (FILE*)(rr->fh));
}

/*****************************************************************************/
size_t seek_file(riff_reader *rr, size_t pos){
	fseek((FILE*)(rr->fh), pos, SEEK_SET);
	return pos;
}

#ifdef RIFF_WRITE
/*****************************************************************************/
size_t seek_writer_file(riff_writer *rw, size_t pos){
	fseek((FILE*)(rw->fh), pos, SEEK_SET);
	return pos;
}

/*****************************************************************************/
size_t write_file(riff_writer *rw, void * ptr, size_t size){
	return fwrite(ptr, sizeof(char), size, (FILE*)rw->fh);
}
#endif

/*****************************************************************************/
//description: see header file
int riff_reader_open_file(riff_reader *rr, FILE *f, size_t size){
	if(rr == NULL)
		return RIFF_ERROR_INVALID_HANDLE;
	rr->fh = f;
	rr->size = size;
	rr->pos_start = ftell(f); //current file offset of stream considered as start of RIFF file
	
	rr->fp_read = &read_file;
	rr->fp_seek = &seek_file;
	
	return riff_readHeader(rr);
}

#ifdef RIFF_WRITE
int riff_writer_open_file(riff_writer *rw, FILE *f) {
	if (rw == NULL)
		return RIFF_ERROR_INVALID_HANDLE;
	rw->fh = f;
	rw->data_size = RIFF_HEADER_SIZE;
	rw->pos_start = ftell(f); //current file offset of stream considered as start of RIFF file
	
	// we are not writing header right now
	rw->pos = rw->fp_seek(rw, RIFF_HEADER_SIZE+rw->pos_start);
	
	rw->fp_write = &write_file;
	rw->fp_seek = &seek_writer_file;
	
	return RIFF_ERROR_NONE;

}

void riff_writer_close_file(riff_writer *rw) {
	if (rw == NULL)
		return;

	int errCode = riff_writeHeader(rw); if (errCode) return;

	return;
}
#endif



//** memory **


/*****************************************************************************/
size_t read_mem(riff_reader *rr, void *ptr, size_t size){
	memcpy(ptr, ((unsigned char*)rr->fh+rr->pos), size);
	return size;
}

/*****************************************************************************/
size_t seek_mem(riff_reader *rr, size_t pos){
	return pos; //instant in memory
}

#ifdef RIFF_WRITE
/*****************************************************************************/
size_t write_mem(riff_writer *rw, void * ptr, size_t size){
	if (rw->pos + size > rw->size) {
		rw->size = fmax((rw->size>>1)*3, 256);
		rw->size = fmax(rw->size, rw->pos + size); 
		rw->fh = realloc(rw->fh, rw->size);
	}

	memcpy(((unsigned char*)rw->fh+rw->pos), ptr, size);
	return size;
}
#endif

/*****************************************************************************/
//description: see header file
int riff_reader_open_mem(riff_reader *rr, void *ptr, size_t size){
	if(rr == NULL)
		return RIFF_ERROR_INVALID_HANDLE;
	
	rr->fh = ptr;
	rr->size = size;
	//rr->pos_start = 0 //redundant -> passed memory pointer is always expected to point to start of riff file
	
	rr->fp_read = &read_mem;
	rr->fp_seek = &seek_mem;
	
	return riff_readHeader(rr);
}

#ifdef RIFF_WRITE
int riff_writer_open_mem(riff_writer *rw){
	if (rw == NULL)
		return RIFF_ERROR_INVALID_HANDLE;
	rw->fh = malloc(256);
	rw->size = 256;
	rw->data_size = RIFF_HEADER_SIZE;
	rw->pos_start = 0;
	
	rw->fp_write = &write_mem;
	rw->fp_seek = (size_t (*) (struct riff_writer *, size_t))&seek_mem;

	// we are not writing header right now, skip past it
	rw->pos = rw->fp_seek(rw, RIFF_HEADER_SIZE);

	rw->state = RIFFWriterStateNotInChunk;
	
	return RIFF_ERROR_NONE;
};

void * riff_writer_close_mem(riff_writer *rw){

	if (rw == NULL){
		if (rw->fp_printf)
			rw->fp_printf(riff_reader_errorToString(RIFF_ERROR_INVALID_HANDLE));
		return NULL;
	}

	int errCode = riff_writeHeader(rw);
	if (errCode) return NULL;

	if (rw->size > rw->data_size){
		rw->size = rw->data_size;
		rw->fh = realloc(rw->fh, rw->size);
	}

	return rw->fh;

};
#endif


// **** internal ****
/*****************************************************************************/
//write 32 bit LE to buffer
void writeBufUInt32LE(void *p, unsigned int value){
	unsigned char *buf = (unsigned char*)p;
	buf[0] = (value		) & 0xFF;
	buf[1] = (value >> 8) & 0xFF;
	buf[2] = (value >>16) & 0xFF;
	buf[3] = (value >>24) & 0xFF;
}

/*****************************************************************************/
//write 32 bit LE to file
void writeUInt32LE(riff_writer *rw, unsigned int value) {
	char buf[4];
	writeBufUInt32LE(buf, value);
	int n = rw->fp_write(rw, buf, 4);
	rw->pos += n;
	rw->c_pos += n;
}


/*****************************************************************************/
//pass pointer to 32 bit LE value and convert, return in native byte order
unsigned int convUInt32LE(void *p){
	unsigned char *c = (unsigned char*)p;
	return c[0] | (c[1] << 8) | (c[2] << 16) | (c[3] << 24);
}


/*****************************************************************************/
//read 32 bit LE from file via FP and return as native
unsigned int readUInt32LE(riff_reader *rr){
	char buf[4] = {0};
	rr->fp_read(rr, buf, 4);
	rr->pos += 4;
	rr->c_pos += 4;
	return convUInt32LE(buf);
}


/*****************************************************************************/
//read chunk header
//return error code
int riff_readChunkHeader(riff_reader *rr){
	char buf[8];
	
	int n = rr->fp_read(rr, buf, 8);
	
	if(n != 8){
		if(rr->fp_printf)
			rr->fp_printf("Failed to read header, %d of %d bytes read!\n", n, 8);
		return RIFF_ERROR_EOF; //return error code
	}
	
	rr->c_pos_start = rr->pos;
	rr->pos += n;
	
	memcpy(rr->c_id, buf, 4);
	rr->c_size = convUInt32LE(buf + 4);
	rr->pad = rr->c_size & 0x1; //pad byte present if size is odd
	rr->c_pos = 0;
	
	
	//verify valid chunk ID, must contain only printable ASCII chars
	int i;
	for(i = 0; i < 4; i++) {
		if(rr->c_id[i] < 0x20  ||  rr->c_id[i] > 0x7e) {
			if(rr->fp_printf)
				rr->fp_printf("Invalid chunk ID (FOURCC) of chunk at file pos %d: 0x%02x,0x%02x,0x%02x,0x%02x\n", rr->c_pos_start, rr->c_id[0], rr->c_id[1], rr->c_id[2], rr->c_id[3]);
			return RIFF_ERROR_ILLID;
		}
	}
	
	
	//check if chunk fits into current list level and file, value could be corrupt
	size_t cposend = rr->c_pos_start + RIFF_CHUNK_DATA_OFFSET + rr->c_size + rr->pad;
	
	size_t listend;
	if(rr->ls_level > 0){
		struct riff_levelStackE *ls = rr->ls + (rr->ls_level - 1);
		listend = ls->c_pos_start + RIFF_CHUNK_DATA_OFFSET + ls->c_size; //end of current list level without pad byte
	}
	else
		listend = rr->pos_start + RIFF_CHUNK_DATA_OFFSET + rr->h_size;
	
	if(cposend > listend){
		if(rr->fp_printf)
			rr->fp_printf("Chunk size exceeds list size! At least one size value must be corrupt!\n");
		//chunk data must be considered as cut off, better skip this chunk
		return RIFF_ERROR_ICSIZE;
	}
	
	//check chunk size against file size
	if((rr->size > 0)  &&  (cposend > rr->size)){
		if(rr->fp_printf)
			rr->fp_printf("Chunk size exceeds file size! At least one size value must be corrupt!\n");
		return RIFF_ERROR_EOF; //Or better RIFF_ERROR_ICSIZE?
	}
	
	return RIFF_ERROR_NONE;
}


/*****************************************************************************/
//pop from level stack
//when returning we are positioned inside the parent chunk ()
void reader_stack_pop(riff_reader *rr){
	if(rr->ls_level <= 0)
		return;
	
	rr->ls_level--;
	struct riff_levelStackE *ls = rr->ls + rr->ls_level;
	
	rr->c_pos_start = ls->c_pos_start;
	memcpy(rr->c_id, ls->c_id, 4);
	rr->c_size = ls->c_size;
	rr->pad = rr->c_size & 0x1; //pad if chunk sizesize is odd
	
	rr->c_pos = rr->pos - rr->c_pos_start - RIFF_CHUNK_DATA_OFFSET;
}


/*****************************************************************************/
//push to level stack
void reader_stack_push(riff_reader *rr, void *type){
	//need to enlarge stack?
	if(rr->ls_size < rr->ls_level + 1){
		size_t ls_size_new = rr->ls_size * 2; //double size
		if(ls_size_new == 0)
			ls_size_new = RIFF_LEVEL_ALLOC; //default stack allocation
		
		struct riff_levelStackE *lsnew = malloc(ls_size_new * sizeof(struct riff_levelStackE));
		rr->ls_size = ls_size_new;
		
		//need to copy?
		if(rr->ls_level > 0){
			memcpy(lsnew, rr->ls, rr->ls_level * sizeof(struct riff_levelStackE));
		}
		
		//free old
		if(rr->ls != NULL)
			free(rr->ls);
		rr->ls = lsnew;
	}
	
	struct riff_levelStackE *ls = rr->ls + rr->ls_level;
	ls->c_pos_start = rr->c_pos_start;
	memcpy(ls->c_id, rr->c_id, 4);
	ls->c_size = rr->c_size;
	//printf("list size %d\n", (rr->ls[rr->ls_level].size));
	memcpy(ls->c_type, type, 4);
	rr->ls_level++;
}

#ifdef RIFF_WRITE
/*****************************************************************************/
//pop from level stack
//when returning we are positioned inside the parent chunk ()
void writer_stack_pop(riff_writer *rw){
	if(rw->ls_level <= 0)
		return;
	
	rw->ls_level--;
	struct riff_levelStackE *ls = rw->ls + rw->ls_level;
	
	rw->pos_start = ls->c_pos_start;
	memcpy(rw->c_id, ls->c_id, 4);
	memcpy(rw->h_type, ls->c_type, 4);
	rw->data_size = fmax(ls->c_size+rw->data_size, rw->pos-rw->pos_start);
	rw->pad = rw->c_size & 0x1; //pad if chunk sizesize is odd
	
	rw->c_pos = rw->pos - rw->c_pos_start - RIFF_CHUNK_DATA_OFFSET;
}


/*****************************************************************************/
//push to level stack
void writer_stack_push(riff_writer *rw){
	//need to enlarge stack?
	if(rw->ls_size < rw->ls_level + 1){
		size_t ls_size_new = rw->ls_size * 2; //double size
		if(ls_size_new == 0)
			ls_size_new = RIFF_LEVEL_ALLOC; //default stack allocation
		
		struct riff_levelStackE *lsnew = malloc(ls_size_new * sizeof(struct riff_levelStackE));
		rw->ls_size = ls_size_new;
		
		//need to copy?
		if(rw->ls_level > 0){
			memcpy(lsnew, rw->ls, rw->ls_level * sizeof(struct riff_levelStackE));
		}
		
		//free old
		if(rw->ls != NULL)
			free(rw->ls);
		rw->ls = lsnew;
	}
	
	struct riff_levelStackE *ls = rw->ls + rw->ls_level;
	ls->c_pos_start = rw->pos_start;
	memcpy(ls->c_id, rw->c_id, 4);
	ls->c_size = rw->data_size;	// data size is what's being incremented
	//printf("list size %d\n", (rw->ls[rw->ls_level].size));
	memcpy(ls->c_type, rw->h_type, 4);
	rw->ls_level++;
}

#endif


//**** user access ****


/*****************************************************************************/
//description: see header file
riff_reader *riff_readerAllocate(){
	riff_reader *rr = calloc(1, sizeof(riff_reader));
	if(rr != NULL){
		rr->fp_printf = riff_printf;
	}
	return rr;
}

/*****************************************************************************/
//description: see header file
//Deallocate riff_reader and contained stack, file source (memory) is not closed or freed
void riff_readerFree(riff_reader *rr){
	if(rr == NULL)
		return;
	//free stack
	if(rr->ls != NULL)
		free(rr->ls);
	//free struct
	free(rr);
}

#ifdef RIFF_WRITE

/*****************************************************************************/
//description: see header file
riff_writer *riff_writerAllocate(){
	riff_writer *rw = calloc(1, sizeof(riff_writer));
	if(rw != NULL){
		rw->fp_printf = riff_printf;
	}
	return rw;
}

/*****************************************************************************/
//description: see header file
//Deallocate riff_writer and contained stack, file source (memory) is not closed or freed
void riff_writerFree(riff_writer *rw){
	if(rw == NULL)
		return;
	//free stack
	if(rw->ls != NULL)
		free(rw->ls);
	//free struct
	free(rw);
}

#endif

/*****************************************************************************/
//description: see header file
//shall be called only once by the open-function
int riff_readHeader(riff_reader *rr){
	char buf[RIFF_HEADER_SIZE];
	
	if(rr->fp_read == NULL) {
		if(rr->fp_printf)
			rr->fp_printf("I/O function pointer not set\n"); //fatal user error
		return RIFF_ERROR_INVALID_HANDLE;
	}
	
	int n = rr->fp_read(rr, buf, RIFF_HEADER_SIZE);
	rr->pos += n;
	
	if(n != RIFF_HEADER_SIZE){
		if(rr->fp_printf)
			rr->fp_printf("Read error, failed to read RIFF header\n");
		//printf("%d", n);
		return RIFF_ERROR_EOF; //return error code
	}
	memcpy(rr->h_id, buf, 4);
	rr->h_size = convUInt32LE(buf + 4);
	memcpy(rr->h_type, buf + 8, 4);


	if(strcmp(rr->h_id, "RIFF") != 0 && strcmp(rr->h_id, "BW64") != 0) {
		if(rr->fp_printf)
			rr->fp_printf("Invalid RIFF header\n");
		return RIFF_ERROR_ILLID;
	}

	int r = riff_readChunkHeader(rr);
	if(r != RIFF_ERROR_NONE)
		return r;

	if (rr->h_size == 0xFFFFFFFF && !memcmp(rr->c_id, "ds64", 4)) {
		// It's a 64-bit sized file
		// Specification can be found at
		// https://www.itu.int/dms_pubrec/itu-r/rec/bs/R-REC-BS.2088-1-201910-I!!PDF-E.pdf
		
		// Buffer already used, so it can be reused
		size_t r_ = riff_readInChunk(rr, buf, 8);
		if (r_ != 8) {
			if (rr->fp_printf) {
				rr->fp_printf("ds64 chunk too small to contain any meaningful information.\n");
			}
			return RIFF_ERROR_ICSIZE;
		}
		rr->h_size = ((size_t)convUInt32LE(buf+4) << 32) | convUInt32LE(buf);
	}
	
	//compare with given file size
	if(rr->size != 0){
		if(rr->size != rr->h_size + RIFF_CHUNK_DATA_OFFSET){
			if(rr->fp_printf)
				rr->fp_printf("RIFF header chunk size %d doesn't match file size %d!\n", rr->h_size + RIFF_CHUNK_DATA_OFFSET, rr->size);
			if(rr->size >= rr->h_size + RIFF_CHUNK_DATA_OFFSET)
				return RIFF_ERROR_EXDAT;
			else
				//end isn't reached yet and you can parse further
				//but file seems to be cut off or given file size (via open-function) was too small -> we are not allowed to read beyond
				return RIFF_ERROR_EOF;
		}
	}

	return RIFF_ERROR_NONE;
}

#ifdef RIFF_WRITE
/*****************************************************************************/
//description: see header file
//shall be called only once by the close function
int riff_writeHeader(riff_writer *rw){
	char buf[RIFF_HEADER_SIZE];


	if(rw->fp_write == NULL) {
		if(rw->fp_printf)
			rw->fp_printf("I/O function pointer not set\n"); //fatal user error
		return RIFF_ERROR_INVALID_HANDLE;
	}


	memcpy(buf, "RIFF", 4);
	writeBufUInt32LE(buf + 4, rw->data_size-RIFF_CHUNK_DATA_OFFSET);
	memcpy(buf + 8, rw->h_type, 4); 

	rw->pos = rw->pos_start;
	seekToPosW(rw);
	int n = rw->fp_write(rw, buf, RIFF_HEADER_SIZE);
	rw->pos += n;

	return RIFF_ERROR_NONE;
}
#endif


// **** external ****



//make use of user defined functions via FPs

/*****************************************************************************/
//read to memory block, returns number of successfully read bytes
//keep track of position, do not read beyond end of chunk, pad byte is not read
size_t riff_readInChunk(riff_reader *rr, void *to, size_t size){
	size_t left = rr->c_size - rr->c_pos;
	if(left < size)
		size = left;
	size_t n = rr->fp_read(rr, to, size);
	rr->pos += n;
	rr->c_pos += n;
	return n;
}

/*****************************************************************************/
//seek byte position in current chunk data from start of chunk data, return error on failure
//keep track of position
//c_pos: relative offset from chunk data start
int riff_readerSeekInChunk(riff_reader *rr, size_t c_pos){
	//seeking behind last byte is valid, next read at that pos will fail
	if(c_pos < 0  ||  c_pos > rr->c_size){
		return RIFF_ERROR_EOC;
	}
	rr->pos = rr->c_pos_start + RIFF_CHUNK_DATA_OFFSET + c_pos;
	rr->c_pos = c_pos;
	size_t r = seekToPosR(rr); //seek never fails, but pos might be invalid to read from
	return RIFF_ERROR_NONE;
}

#ifdef RIFF_WRITE
// write in current chunk
size_t riff_writeInChunk(riff_writer *rw, void *from, size_t size){
	if (rw->state == RIFFWriterStateNotInChunk) return RIFF_WRITER_ERROR_NOTCHUNK;
	size_t n = rw->fp_write(rw, from, size);
	rw->pos += n;
	rw->c_pos += n;
	rw->data_size = fmax(rw->data_size, rw->pos-rw->pos_start);
	rw->c_size = fmax(rw->c_size, rw->c_pos);
	return n;
}
//seek in current chunk, returns RIFF_ERROR_EOC if end of chunk is reached, pos 0 is first byte after chunk size (chunk offset 8)
int riff_writerSeekInChunk(riff_writer *rw, size_t c_pos){
	if (rw->state == RIFFWriterStateNotInChunk) return RIFF_WRITER_ERROR_NOTCHUNK;
	//seeking behind last byte is valid, next read at that pos will fail
	if(c_pos < 0  ||  c_pos > rw->c_size){
		return RIFF_ERROR_EOC;
	}
	rw->pos = rw->c_pos_start + RIFF_CHUNK_DATA_OFFSET + c_pos;
	rw->c_pos = c_pos;
	size_t r = seekToPosW(rw); //seek never fails, but pos might be invalid to read from
	return RIFF_ERROR_NONE;
}
#endif


/*****************************************************************************/
//description: see header file
int riff_readerSeekNextChunk(riff_reader *rr){
	size_t posnew = rr->c_pos_start + RIFF_CHUNK_DATA_OFFSET + rr->c_size + rr->pad; //expected pos of following chunk
	
	size_t listend;
	if(rr->ls_level > 0){
		struct riff_levelStackE *ls = rr->ls + (rr->ls_level - 1);
		listend = ls->c_pos_start + RIFF_CHUNK_DATA_OFFSET + ls->c_size; //end of current list level without pad byte
	}
	else
		listend = rr->pos_start + RIFF_CHUNK_DATA_OFFSET + rr->h_size; //at level 0
	
	//printf("listend %d  posnew %d\n", listend, posnew);  //debug
	
	//if no more chunks in the current sub list level
	if(listend < posnew + RIFF_CHUNK_DATA_OFFSET){
		//there shouldn't be any pad bytes at the list end, since the containing chunks should be padded to even number of bytes already
		//we consider excess bytes as non critical file structure error
		if(listend > posnew){
			if(rr->fp_printf)
				rr->fp_printf("%d excess bytes at pos %d at end of chunk list!\n", listend - posnew, posnew);
			return RIFF_ERROR_EXDAT;
		}
		return RIFF_ERROR_EOCL;
	}
	
	rr->pos = posnew;
	rr->c_pos = 0; 
	seekToPosR(rr);
	
	return riff_readChunkHeader(rr);
}


/*****************************************************************************/
int riff_readerSeekChunkStart(struct riff_reader *rr){
	//seek data offset 0 in current chunk
	rr->pos = rr->c_pos_start + RIFF_CHUNK_DATA_OFFSET;
	rr->c_pos = 0;
	seekToPosR(rr);
	return RIFF_ERROR_NONE;
}


/*****************************************************************************/
int riff_readerRewind(struct riff_reader *rr){
	//pop stack as much as possible
	while(rr->ls_level > 0) {
		reader_stack_pop(rr);
	}
	return riff_readerSeekLevelStart(rr);
}

/*****************************************************************************/
int riff_readerSeekLevelStart(struct riff_reader *rr){
	//if in sub list level
	if(rr->ls_level > 0)
		rr->pos = rr->ls[rr->ls_level - 1].c_pos_start;
	else
		rr->pos = rr->pos_start;
		
	rr->pos += RIFF_CHUNK_DATA_OFFSET + 4; //pos after type ID of chunk list
	rr->c_pos = 0;
	seekToPosR(rr);

	//read first chunk header, so we have the right values
	int r = riff_readChunkHeader(rr);
	
	//check possible?
	return r;
}


/*****************************************************************************/
//description: see header file
int riff_readerSeekLevelSub(riff_reader *rr){
	//according to "https://en.wikipedia.org/wiki/Resource_Interchange_File_Format" only RIFF and LIST chunk IDs can contain subchunks
	if(strcmp(rr->c_id, "LIST") != 0  && strcmp(rr->c_id, "RIFF") != 0 && strcmp(rr->c_id, "BW64") != 0){
		if(rr->fp_printf)
			rr->fp_printf("%s() failed for chunk ID \"%s\", only RIFF or LIST chunk can contain subchunks\n", __func__, rr->c_id);
		return RIFF_ERROR_ILLID;
	}
	
	//check size of parent chunk data, must be at least 4 for type ID (is empty list allowed?)
	if(rr->c_size < 4){
		if(rr->fp_printf)
			rr->fp_printf("Chunk too small to contain sub level chunks\n");
		return RIFF_ERROR_ICSIZE;
	}
	
	//seek to chunk start if not there, required to read type ID
	if(rr->c_pos > 0) {
		rr->pos = rr->c_pos_start + RIFF_CHUNK_DATA_OFFSET;
		seekToPosR(rr);
		rr->c_pos = 0;
	}
	//read type ID
	unsigned char type[5] = "\0\0\0\0\0";
	rr->fp_read(rr, type, 4);
	rr->pos += 4;
	//verify type ID
	int i;
	for(i = 0; i < 4; i++) {
		if(type[i] < 0x20  ||  type[i] > 0x7e) {
			if(rr->fp_printf)
				rr->fp_printf("Invalid chunk type ID (FOURCC) of chunk at file pos %d: 0x%02x,0x%02x,0x%02x,0x%02x\n", rr->c_pos_start, type[0], type[1], type[2], type[3]);
			return RIFF_ERROR_ILLID;
		}
	}
	
	//add parent chunk data to stack
	//push
	reader_stack_push(rr, type);
	
	return riff_readChunkHeader(rr);
}


#ifdef RIFF_WRITE
int riff_writerNewChunk(struct riff_writer *rw){
	if (rw->state == RIFFWriterStateInChunk) {
		if (rw->fp_printf) rw->fp_printf("Currently in chunk, cannot start new chunk\n"); 
		return -1;
	}
	// Assumes that it is in freespace, after a finished chunk
	rw->c_pos_start = rw->pos;
	
	// Reserve bytes for header
	char tmp[8];
	int n = rw->fp_write(rw, tmp, RIFF_CHUNK_DATA_OFFSET);
	rw->pos += n;
	rw->data_size = fmax(rw->data_size, rw->pos-rw->pos_start);

	rw->c_size = 0;
	rw->c_pos = 0;

	rw->state = RIFFWriterStateInChunk;
	
	return RIFF_ERROR_NONE;
}

int riff_writerFinishChunk(struct riff_writer *rw){
	if (rw->state == RIFFWriterStateNotInChunk) return RIFF_WRITER_ERROR_NOTCHUNK;
	for(int i = 0; i < 4; i++) {
		if(rw->c_id[i] < 0x20  ||  rw->c_id[i] > 0x7e) {
			if(rw->fp_printf)
				rw->fp_printf("Invalid chunk type ID (FOURCC): 0x%02x,0x%02x,0x%02x,0x%02x\n", rw->c_id[0], rw->c_id[1], rw->c_id[2], rw->c_id[3]);
			return RIFF_ERROR_ILLID;
		}
	}

	// Overall gist: 
	// write size, id, and seek to the first byte after this chunk

	// Seek to start of chunk
	rw->pos = rw->c_pos_start;
	seekToPosW(rw);

	// Write ID
	size_t n = rw->fp_write(rw, rw->c_id, 4);
	rw->pos += n;
	// Write size
	writeUInt32LE(rw, rw->c_size);

	// Seek to start of chunk
	rw->pos = rw->c_pos_start + RIFF_CHUNK_DATA_OFFSET + rw->c_size;
	seekToPosW(rw);

	// Adjust for possible pad byte
	rw->pad = rw->c_size & 0x1; //pad byte present if size is odd
	if (rw->pad) {
		char tmp = 0;
		rw->fp_write(rw, &tmp, 1);
		rw->pos++;
		rw->data_size = fmax(rw->data_size, rw->pos-rw->pos_start);
	}

	rw->state = RIFFWriterStateNotInChunk;

	return RIFF_ERROR_NONE;
}

int riff_writerNewListChunk(struct riff_writer *rw){
	if (rw->state == RIFFWriterStateInChunk) {
		if (rw->fp_printf) rw->fp_printf("Currently in chunk, cannot start chunk list\n"); 
		return -1;
	}
	writer_stack_push(rw);

	rw->pos_start = rw->pos;
	// ID automatically set to LIST since it's the only one allowed
	size_t n = rw->fp_write(rw, "LIST", 4);
	rw->pos += n;
	
	//reserve 8 other bytes
	char tmp[8];
	n = rw->fp_write(rw, tmp, 8);
	rw->pos += n;
	// rw->c_pos = rw->pos - rw->pos_start - RIFF_CHUNK_DATA_OFFSET;
		
	//add parent chunk data to stack
	//push

	rw->data_size = RIFF_HEADER_SIZE;
	
	return RIFF_ERROR_NONE;
}       //reserve space for new list chunk after the previous one, go to sub level

//step back from sub list level; position changes to after this list chunk, just like riff_writerFinishChunk
//returns != RIFF_ERROR_NONE, if we are at level 0 already and can't go back any further
int riff_writerFinishListChunk(struct riff_writer *rw){
	if(rw->ls_level <= 0){
		if (rw->fp_printf) rw->fp_printf("Currently at the top level, no list chunks to finish; did you mean to close the file?\n"); 
		return -1;  //not critical error, we don't have or need a macro for that
	}

	// Write chunk data
	rw->pos = rw->pos_start+4;
	seekToPosW(rw);	// Go to beginning of data to change

	// Write length
	writeUInt32LE(rw, rw->data_size-RIFF_CHUNK_DATA_OFFSET);
	// pos automatically updated

	// Write type
	int n = rw->fp_write (rw, rw->h_type, 4);
	rw->pos += n;

	// Seek to start of chunk
	rw->pos = rw->pos_start + rw->data_size;
	seekToPosW(rw);

	// Adjust for possible pad byte
	rw->pad = rw->data_size & 0x1; //pad byte present if size is odd
	if (rw->pad) {
		char tmp = 0;
		rw->fp_write(rw, &tmp, 1);
		rw->pos++;
	}

	// Pop the parent level data off the stack (WHY IS THIS SEPARATE???)
	writer_stack_pop(rw);
	return RIFF_ERROR_NONE;
}


int riff_writerSeekChunkStart(struct riff_writer *rw){	
	//seek data offset 0 in current chunk
	rw->pos = rw->c_pos_start + RIFF_CHUNK_DATA_OFFSET;
	rw->c_pos = 0;
	seekToPosW(rw);
	return RIFF_ERROR_NONE;
}

int riff_writerRewind(struct riff_writer *rw);              //seek back to very first chunk of file at level 0, the position just after opening via riff_open_...()
int riff_writerSeekLevelStart(struct riff_writer *rw);      //goto start of first data byte of first chunk in current level (seek backward)
int riff_writerSeekLevelSub(struct riff_writer *rw);        //goto sub level chunk (auto seek to start of parent chunk if not already there); "LIST" chunk typically contains a list of sub chunks


#endif

/*****************************************************************************/
//description: see header file
int riff_readerLevelParent(struct riff_reader *rr){
	if(rr->ls_level <= 0)
		return -1;  //not critical error, we don't have or need a macro for that
	reader_stack_pop(rr);
	return RIFF_ERROR_NONE;
}


/*****************************************************************************/
int riff_readerLevelValidate(struct riff_reader *rr){
	int r;
	//seek to start of current list
	if((r = riff_readerSeekLevelStart(rr))  !=  RIFF_ERROR_NONE)
		return r;
	
	//seek all chunks of current list level
	while(1){
		r = riff_readerSeekNextChunk(rr);
		if(r != RIFF_ERROR_NONE){
			if(r == RIFF_ERROR_EOCL) //just end of list
				break;
			//error occured, was probably printed already
			return r;
		}
	}
	return RIFF_ERROR_NONE;
}


/*****************************************************************************/
//description: see header file
const char *riff_reader_errorToString(int e){
	//map error to error string
	//Make sure mapping is correct!
	return riff_reader_es[e > RIFF_ERROR_UNKNOWN ? RIFF_ERROR_UNKNOWN : e];
}

#ifdef RIFF_WRITE

//description: see header file
const char *riff_writer_errorToString(int e){
	//map error to error string
	//Make sure mapping is correct!
	return riff_writer_es[e > RIFF_ERROR_UNKNOWN ? RIFF_ERROR_UNKNOWN : e];
}

#endif