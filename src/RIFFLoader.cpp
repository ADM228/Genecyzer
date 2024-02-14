#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sys/types.h>
#include <vector>

#include "RIFF.cpp"
#include "BitConverter.cpp"
#include "RIFF.hpp"
#include "Tracker.cpp"
#include "Utils.cpp"

#include "Project.cpp"
#include "Song.cpp"
#include "Instrument.cpp"
#include "Effect.cpp"

// Var16 number format:

/*
Treated as 
!!! BIG-ENDIAN !!!
2 bytes if the value of the first
byte < 0x40, else it's treated as 1
byte (and 0x40 is subtracted)
*/

#define VAR16_MIN_SIZE 1
#define VAR16_MAX_SIZE 2

#define VAR16_MIN_VALUE 0
#define VAR16_MAX_VALUE 0x3FFF
#define VAR16_ROLLOVER_VALUE (0x100 - 0x40)


uint16_t decodeVar16 (uint8_t * & ptr) {
	uint16_t value; 
	if (*ptr < 0x40) {
		value = ((*ptr) << 8) | (*(ptr+1) & 0xFF);
		ptr += 2;
	} else {
		value = (*ptr) - 0x40;
		ptr++;
	}
	return value;
}

std::vector <uint8_t> encodeVar16 (uint16_t value) {
	if (value < VAR16_ROLLOVER_VALUE)
		return std::vector<uint8_t> {static_cast<uint8_t>((value+0x40)&0xFF)};
	else
		return std::vector<uint8_t> {static_cast<uint8_t>(((value>>8)&0x3F)), static_cast<uint8_t>((value&0xFF))};
}

int getSizeOfVar16 (uint16_t value) {
	return value < VAR16_ROLLOVER_VALUE ? 1 : 2;
}

template <typename T> void writeVar16 (T * ptr, uint16_t value) {
	uint8_t * ptr8 = ptr;
	if (value < VAR16_ROLLOVER_VALUE)
		*ptr8 = (uint8_t)(value+0x40);
	else {
		*ptr8 = (uint8_t)((value>>8) & 0x3F);
		*(ptr8+1) = (uint8_t)(value & 0xFF);
	}
}

// Project file format:

/* It's a RIFF file, with the gczr file extension
    The RIFF specification can be found at 
        thirdparty/libriff/docs/riffmci.pdf
    Chunk types:
        "RIFF" - the root of everything, as per the RIFF spec:
            4 bytes [00:03] -
                The string "RIFF". The loading will be aborted
                if the string doesn't match.
            4 bytes [04:07] - 
                The length of the data in the chunk.
            4 bytes [08:11] - (counts as data)
                The string indicating the file format. It
                should be "GCZR" for Genecyzer, the loading
                will be aborted if it doesn't match that.
            X bytes [12:] - (counts as data)
                The rest of the data in the chunk, which is the
                entire file.

        For the rest of the chunk types' descriptions, the
        chunk header is omitted.

        "ver " - tells about the file version:
            8 bytes [08:15] - 
                the branch of Genecyzer where this file was
                saved from. Full Genecyzer releases only accept
                "Release ", and in development versions on the
                main branch also accept "Dev Main", otherwise
                loading will be aborted. If you are creating a
                fork with incompatible file data, please use a
                different combination of 8 bytes, otherwise
                stuff might break.
            4 bytes [16:19] -
                the version of the files inside that branch.

        "LIST" - The list chunk as per the RIFF spec. Several
        types are in use by Genecyzer:

        Type "INFO" - as per RIFF spec. Only the following 
        subchunks are used by Genecyzer (the rest are ignored
        on reading):
            "IART" - Artist. 
                Lists the contents of the composer field.
            "ICMT" - Comments.
                Lists the contents of the notes field. Since
                the RIFF spec explicitly says not to include 
                newline characters, they are converted into the
                Record Separator characters (0x1E) when writing
                to the file, and back into newline characters
                when reading from it.
            "ICOP" - Copyright.
                Lists the contents of the copyright field.
            "ICRD" - Creation date.
                Lists the first time the file has been saved.
            "INAM" - Name.
                Lists the title of the project.
            "ISFT" - Software.
                Genecyzer always sets this field to 
                "Genecyzer", gives a warning if the string
                doesn't match it on reading.

        Type "song" - the internal Genecyzer song format. 
        Contains the following subchunks:
            "effc" - The amount of effect columns:
                8 bytes [08:15] - the amount of effect columns
                for the corresponding channel.
            "INAM" - Name. 
                Lists the song title.
            "col " - Color.
                Lists the color of the song in the standard
                24-bit color format.
            "pat " - a pattern (the indexes are assumed	to be
			in order):
				4 bytes  [08:11] - The amount of rows in the
				pattern.
				16 bytes [12:27] - The pattern indexes
				themselves, 16-bit words.
                Var16 bytes - Size of the list of major beats.
				X bytes - The major beats.
				Var16 bytes - Size of the list of minor beats.
                X bytes - The minor beats.
            "note" chunk - a chunk of a "note struct":
                4 bytes - the amount of note structs in 
                this chunk. Each note struct consists of:
                    1 byte - the note value:
                        In range 0..95 for C0..B7, 
                        253 means to repeat the default 
                        tracker cell (and no further data
                        follows after this),
                        254 means a KEY OFF/stop note,
                        255 means an empty note cell.
                    1 byte - the flags for the following
                    data:
                        bit 7 - whether the note has attack
                        enabled,
                        bit 6 - whether an instrument value
                        is set,      
                        bit 5 - whether any effects are 
                        declared,
                        bit 4 - whether to set this cell as
                        the default cell.
						bit 3 - whether to repeat the note
						on the next cells.
						bit 2 - whether to repeat the 
						instruments on the next cells.
						bit 1 - whether to repeat the flag
						byte (except for repeat bits) on 
						the next X cells.
                    1 byte (optional) - Instrument value
                        Only present if bit 6 is set in the
                        flags byte.
                    X bytes (optional) - Effect data
                        TODO this
					1-2 bytes (optional) - Repeat count
					(note)
						Only present if bit 3 is set in the
						flags byte. Var16 format.
					1-2 bytes (optional) - Repeat count 
					(instrument)
						Only present if bit 2 is set in the
						flags byte. Var16 format.
					1-2 bytes (optional) - Repeat count 
					(flag byte)
						Only present if bit 1 is set in the
						flags byte. Var16 format.

        
*/

namespace RIFFLoader {

Song loadSongFromRIFF(RIFF::RIFFReader & file);

std::vector<TrackerCell> decodeNoteStruct (std::vector<uint8_t> * chunkData);
std::vector<uint8_t> encodeNoteStruct (std::vector<TrackerCell> & pattern);

TrackerPattern decodePatternStruct (std::vector<uint8_t> * chunkData);
std::vector<uint8_t> encodePatternStruct (TrackerPattern & pattern);


// RIFF constants

const char fileType     [5]     = "GCZR";
const char software     [10]    = "Genecyzer";

const char mainBranch   [9]     = "Release ";
const char thisBranch   [9]     = "Dev Main";


// Chunk IDs
const char versionId        [5]     = "ver ";
const char listId           [5]     = "LIST";
// in "INFO"
const char artistId         [5]     = "IART";
const char commentsId       [5]     = "ICMT";
const char copyrightId      [5]     = "ICOP";
const char creationDateId   [5]     = "ICRD";
const char nameId           [5]     = "INAM";
const char softwareId       [5]     = "ISFT";
// in "song"
const char effectColumnId   [5]     = "effc";
const char * songNameId             = nameId;
const char colorId          [5]     = "col ";
const char noteId           [5]     = "note";
const char patternId		[5]		= "ptrn";

// List types
const char infoListType		[5]		= "INFO";
const char songListType		[5]		= "song";

const uint32_t mainBranchVer = 0;
const uint32_t thisBranchVer = 0;

#define ifEq(ptra, ptrb) if (!memcmp(ptra, ptrb, 4))
#define ifNeq(ptra, ptrb) if (memcmp(ptra, ptrb, 4))

#define getID() file.rr->c_id
#define getType() file.rr->ls[file.rr->ls_level-1].c_type

#define ifID(ptr) ifEq(id, ptr)
#define ifTp(ptr) ifEq(type, ptr)


int loadRIFFFile (RIFF::RIFFReader & file, Project & project) {
	project = Project();

	int errCode;
	char *id, *type;

	// 1. Test file type
		ifNeq (file.rr->h_type, fileType){
			fprintf(stderr, "The file type is not a Genecyzer file. Aborting loading\n");
			return -1;
		} 
		auto chunkData = file.readChunkData();
		if (chunkData == nullptr) return RIFF_ERROR_UNKNOWN;
		auto data = chunkData->data();
		auto size = chunkData->size();
		printByteArray(data, size, 16);
		if ( !(
			(!memcmp(data, mainBranch, 8) && readUint32(data+8) <= mainBranchVer) || 
			(!memcmp(data, thisBranch, 8) && readUint32(data+8) <= thisBranchVer)
		) ) { 
			fprintf(stderr, "File version is invalid. Aborting loading\n");
			return -1;
		}
		delete chunkData;
		errCode = file.seekNextChunk();

    // And now, read the rest of the file
    while (!errCode) {
		id = getID();
        ifID(listId){
            // LIST type, has several subtypes
            errCode = file.seekLevelSub();

            auto * type = getType();

            ifTp (infoListType) {
				// INFO subchunk
				while (!errCode) {
					chunkData = file.readChunkData();

					id = getID();

					ifID(commentsId) {
					// Comment subsubchunk, gotta convert 'em
					// record separator chars into newlines
					for (auto &c : *chunkData) {
						if (c == 0x1E)
						c = 0x0A; // Record Separator -> LineFeed
					}
					}

					printByteArray(chunkData->data(), chunkData->size(), 16);

					if (chunkData->back() != 0)
					chunkData->push_back(0);

					ifID(nameId) project.name() =
						std::string((char *)chunkData->data());
					else ifID(artistId) project.composer() =
						std::string((char *)chunkData->data());
					else ifID(copyrightId) project.copyright() =
						std::string((char *)chunkData->data());
					else ifID(commentsId) project.comments() =
						std::string((char *)chunkData->data());
					else ifID(softwareId) {
					if (!(chunkData->size() == 10 &&
							!memcmp(chunkData->data(), software, 9)))
						fprintf(stderr,
								"The \"Software\" field in the Genecyzer file's "
								"metadata is not set to \"Genecyzer\". This "
								"indicates a file that has been created or "
								"modified by external software, which could lead "
								"to invalid file loading.\n");
					}
					delete chunkData;

					errCode = file.seekNextChunk();
				}
				if (errCode != RIFF_ERROR_EOCL) {fprintf(stderr, "%s", file.errorToString(errCode).c_str());}
            } else ifTp(songListType) {
				project.songs.push_back(loadSongFromRIFF(file));
			}
			errCode = file.levelParent(); if (errCode) {fprintf(stderr, "%s", file.errorToString(errCode).c_str());}
			errCode = file.seekChunkStart(); if (errCode) {fprintf(stderr, "%s", file.errorToString(errCode).c_str());}

        } 

        errCode = file.seekNextChunk();
    }

    printf("Name: %s\nComposer: %s\nCopyright:\n----\n%s\n----\nComments:\n----\n%s\n----\n", project.name().c_str(), project.composer().c_str(), project.copyright().c_str(), project.comments().c_str());

	return errCode == RIFF_ERROR_EOCL ? 0 : errCode;
}

int saveRIFFFile (RIFF::RIFFWriter & file, Project & project) {
	// Write the version chunk
	file.newChunk();
	file.writeInChunk((void *)thisBranch, 8);
	char buf[4];
	writeBytes(thisBranchVer, buf);
	file.writeInChunk(buf, 4);
	file.finishChunk((char *)versionId);    

	// Write the INFO LIST chunk
	file.newListChunk((char *)infoListType);
	{
		auto metadata = project.exportMetadata();
		auto & name 		= metadata.name;
		auto & composer 	= metadata.composer;
		auto & copyright	= metadata.copyright;
		auto & comments	= metadata.comments;

		file.writeNewChunk((void*)software, sizeof(software), (char *)softwareId);
		if (composer.length())
			file.writeNewChunk((void *)composer.c_str(), composer.length(), (char *)artistId);
		if (comments.length()) {
			char * chunkData = new char[comments.length()];
			std::strncpy(chunkData, comments.c_str(), comments.length());
			for (size_t i = 0; i < comments.length(); i++) {
				if (chunkData[i] == 0x0A) chunkData[i] = 0x1E; // LineFeed -> Record Separator
			}
			file.writeNewChunk(chunkData, comments.length(), (char *)commentsId);
			delete[] chunkData;
		}
		if (copyright.length())
			file.writeNewChunk((void *)copyright.c_str(), copyright.length(), (char *)copyrightId);
		if (name.length())
			file.writeNewChunk((void *)name.c_str(), name.length(), (char *)nameId);
	}

	file.finishListChunk();

	for (Song &song : project.songs) {
		file.newListChunk((char *)songListType);

			file.writeNewChunk(project.songs[0].effectColumnAmount.data(), 8, (char *)effectColumnId);

			for (auto &pattern : song.patternData) {
				if (pattern.size() > 0) {
					auto data = encodeNoteStruct(pattern);

					file.writeNewChunk(data, (char *)noteId);
				}
			}

			for (auto &pattern : song.patterns) {
				auto data = encodePatternStruct(pattern);

				file.writeNewChunk(data, (char *)patternId);
			}

		file.finishListChunk();
	}

	file.setFileType((char *)fileType);

	return 0;
}

Song loadSongFromRIFF(RIFF::RIFFReader & file) {
	char * id = getID();
	char * type = getType();

	if (memcmp(type, songListType, 4)){err ("RLoad:LSong: TP WRONG"); return *(Song*)0;};

	Song song;

	// Format is correct, let's start loading

	int errCode = 0;

	while (!errCode) {
		id = getID();
		ifID (effectColumnId) {
			auto data = file.readChunkData();
			if (data == nullptr) {err ("RLoad:LSong: EFFC RCD NULLPTR\n");}
			else if (data->size() != 8) {err ("RLoad:LSong: EFFC RCD SIZE\n");}
			else memcpy(&song.effectColumnAmount, data->data(), 8);	// Is endian-safe cuz 1 byte
			delete data;
		} else ifID (noteId) {
			auto data = file.readChunkData();
			if (data == nullptr) {err ("RLoad:LSong: NOTE RCD NULLPTR\n");}
			else if (data->size() < 4) {err ("RLoad:LSong: NOTE RCD SIZE\n");}
			else song.patternData.push_back(decodeNoteStruct(data));
			delete data;
		} else ifID (patternId) {
			auto data = file.readChunkData();
			if (data == nullptr) {err ("RLoad:LSong: PTRN RCD NULLPTR\n");}
			else if (data->size() < 2*sizeof(uint16_t)+2*VAR16_MIN_SIZE+sizeof(uint32_t)) {err ("RLoad:LSong: PTRN RCD SIZE\n");}
			else song.patterns.push_back(decodePatternStruct(data));
			delete data;
		}

		errCode = file.seekNextChunk();
	};
	if (errCode != RIFF_ERROR_EOCL) {fprintf(stderr, "%s", file.errorToString(errCode).c_str());}

	return song;
};

#define REPEAT_DEFAULT_CELL 253

#define ATTACK 7
#define INSTRUMENT 6
#define EFFECTS 5
#define SET_DEFAULT 4
#define NOTE_REPEAT 3
#define INST_REPEAT 2
#define FLAG_REPEAT 1

std::vector<TrackerCell> decodeNoteStruct (std::vector<uint8_t> * chunkData) {
	// Accepts chunk data directly from RIFF::RIFFReader::ReadChunkData()
	if (chunkData == 0) return std::vector<TrackerCell>(0);
	uint8_t * ptr = chunkData->data();	// Might seem unnecessary, but this will prevent segfaults
	
	uint32_t count = readUint32(ptr);
	if (count == 0) return std::vector<TrackerCell>(0);
	ptr += sizeof(count);
	
	uint16_t noteRptCount = 0, instRptCount = 0, flagRptCount = 0;
	uint8_t noteRpt, instRpt, flagRpt;
	
	uint8_t * endPtr = chunkData->data()+chunkData->size();
	TrackerCell cell, defaultCell;
	std::vector<TrackerCell> array;

	for (int i = count; ptr <= endPtr && i != 0; i--) {
		// 1. Parse (or repeat) the note byte
		uint8_t noteValue;

		if (noteRptCount != 0) {
			noteRptCount--;
			noteValue = noteRpt;
		} else {
			noteValue = *ptr;
			ptr++;
		}

		if (noteValue == REPEAT_DEFAULT_CELL) {
			array.push_back(defaultCell);
			continue;
		} else if (noteValue >= 0 && noteValue <= MAX_NOTE || noteValue == EMPTY_NOTE || noteValue == KEY_OFF) cell.noteValue = noteValue;

		// 2. Parse the flags byte
		uint8_t flagsByte;
		if (flagRptCount != 0) {
			flagRptCount--;
			flagsByte = flagRpt;
		} else {
			flagsByte = *ptr;
			ptr++;
		}

		cell.attack((flagsByte & 1<<ATTACK) != 0);	// 3. Set attack
		if (flagsByte & 1<<INSTRUMENT) {
			// 4. Get instrument value
			if (instRptCount != 0) {
				instRptCount--;
				cell.instrument = instRpt;
			} else {
				cell.instrument = *ptr;
				ptr++;
			}
			cell.hideInstrument(false);
		} else cell.hideInstrument(true);
		if (flagsByte & 1<<EFFECTS) {
			// 5. Effects
			// ptr++;
			err ("No FX support rn\n");
		}
		if (flagsByte & 1<<SET_DEFAULT) {
			// 6. Set it as the default cell
			defaultCell = cell;
		}

		// 7. Deal with repeating
		if (flagsByte & 1<<NOTE_REPEAT) {
			// 7.1. Note repeating
			noteRpt = noteValue;
			noteRptCount = decodeVar16(ptr);
		}
		if (flagsByte & 1<<INST_REPEAT) {
			// 7.2. Instrument repeating
			instRpt = cell.instrument;
			instRptCount = decodeVar16(ptr);
		}
		if (flagsByte & 1<<FLAG_REPEAT) {
			// 7.3. Flag byte repeating
			flagRpt = flagsByte;
			flagRpt &= ~((1<<NOTE_REPEAT)|(1<<INST_REPEAT)|(1<<FLAG_REPEAT)); 	// Repeating this would break shit
			flagRptCount = decodeVar16(ptr);

		}

		array.push_back(cell);
	}

	return array;
}

std::vector<uint8_t> encodeNoteStruct (std::vector<TrackerCell> & pattern) {
	// Accepts chunk data directly from RIFF::RIFFReader::ReadChunkData()
	std::vector<uint8_t> array (4);
		
	writeBytes((uint32_t)pattern.size(), array.data());
	if (pattern.size() == 0) {return array;}
	
	for (auto & cell : pattern) {
		array.push_back(cell.noteValue);
		array.push_back(
			(cell.attack() 			? 1<<ATTACK : 0		) |
			(cell.hideInstrument()	? 0 : 1<<INSTRUMENT	)
		);
		if (!cell.hideInstrument()) 
			array.push_back(cell.instrument);
	}

	//TODO COMPRESSION AGLORITHM

	return array;
}

TrackerPattern decodePatternStruct (std::vector<uint8_t> * chunkData) {
	TrackerPattern pattern;

	auto * ptr = chunkData->data();	// Might seem unnecessary, but this will prevent segfaults

	// Get amount of rows
	pattern.rows = readUint32(ptr);
	ptr += sizeof(uint32_t);

	// Get the rows
	for (size_t i = 0; i < 8; i++, ptr += sizeof(uint16_t))
		pattern.cells[i] = readUint16(ptr);

	// Major beats
	auto count = decodeVar16(ptr);
	pattern.beats_major = std::vector<uint16_t> (count);
	for (size_t i = 0; i < count; i++, ptr += sizeof(uint16_t))
		pattern.beats_major[i] = readUint16(ptr);

	// Minor beats
	count = decodeVar16(ptr);
	pattern.beats_minor = std::vector<uint16_t> (count);
	for (size_t i = 0; i < count; i++, ptr += sizeof(uint16_t))
		pattern.beats_minor[i] = readUint16(ptr);
	
	return pattern;
}


std::vector<uint8_t> encodePatternStruct (TrackerPattern & pattern) {
	std::vector<uint8_t> array (sizeof(uint16_t)*8+sizeof(uint32_t));

	size_t offset = 0;
	auto * ptr = array.data();	

	// Row amount
	writeBytes((uint32_t)pattern.rows, ptr+offset);
	offset += sizeof(uint32_t);

	// Put the rows
	for (size_t i = 0; i < 8; i++, offset+=sizeof(uint16_t))
		writeBytes(pattern.cells[i], ptr+offset);

	// Major beats
	auto sizeOfData = pattern.beats_major.size();
	auto sizeOfSize = getSizeOfVar16(sizeOfData);
	array.resize(array.size()+sizeOfSize+sizeOfData*sizeof(uint16_t));
	ptr = array.data();	// Update cuz might have moved

	writeVar16(ptr+offset, sizeOfData);
	offset += sizeOfSize;

	for (size_t i = 0; i < sizeOfData; i++, offset+=sizeof(uint16_t))
		writeBytes(pattern.beats_major[i], ptr+offset);

	// Minor beats
	sizeOfData = pattern.beats_minor.size();
	sizeOfSize = getSizeOfVar16(sizeOfData);

	array.resize(array.size()+sizeOfSize+sizeOfData*sizeof(uint16_t));
	ptr = array.data();	// Update cuz might have moved

	writeVar16(ptr+offset, sizeOfData);
	offset += sizeOfSize;

	for (size_t i = 0; i < sizeOfData; i++, offset+=sizeof(uint16_t))
		writeBytes(pattern.beats_minor[i], ptr+offset);
	
	return array;
}

}	// namespace RIFFLoader