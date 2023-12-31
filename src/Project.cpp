#ifndef __PROJECT_INCLUDED__
#define __PROJECT_INCLUDED__

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <array>
#include "TextRenderer.cpp"
#include "Instrument.cpp"
#include "Tracker.cpp"
#include "RIFF.cpp"
#include "BitConverter.cpp"
#include "Utils.cpp"
#include "libriff/riff.h"

struct TrackerPattern {
    std::array<uint32_t, 8> cells;
    std::vector<uint16_t> beats_major;
    std::vector<uint16_t> beats_minor;
    size_t rows;
};

class FileException : public std::exception {
    public:
        FileException (const char * what_arg) : std::exception() {
            std::string output = "Reading a file has failed: ";
            output += what_arg;
            message = new char[output.size()+1];
            strcpy(message, output.c_str());    // Unsafe as fuck and idc
        };

        ~FileException () {
            delete[] message;
        }

        virtual const char * what () const noexcept {
            return message;
        }

    private:
        char * message;
};

class Project {
    public:
        // Create new project
        Project ();

        // Load project from filestream
        int Load (std::ifstream & file);
        // Load project from memory
        int Load (std::vector<uint8_t>& data);

        // Save project to filestream
        void Save (std::ofstream file);
        // Save project to memory
        uint8_t * Save (size_t * size_out);

        // Export project's patterns to SNESFM opcode format
        uint8_t * exportSNESFM ();

        // The very cells
        std::vector<TrackerPattern> patterns;

        std::vector<std::vector<TrackerCell>> patternData;

        std::array<uint8_t, 8> effectColumnAmount; 

        std::vector<Instrument> instruments;

    private:

        int loadInternal (RIFF::RIFFReader & file);

        std::string name;
        std::string composer;
        std::string copyright;
        std::string comments;


};

Project::Project() {
    TrackerPattern defaultPattern {
        {0, 0, 0, 0, 0, 0, 0, 0},
        std::vector<uint16_t> {16},
        std::vector<uint16_t> {4},
        64
    };
    patterns.push_back(defaultPattern);
    patternData.push_back(std::vector<TrackerCell>(64));
    effectColumnAmount.fill(2);


    const std::string allTextTest = "English Ελληνικά Русский にほんこご antidisestablishmentarianism\nThe quick brown fox jumped over the lazy dog\nСъешь же ещё этих мягких французских булочек, да выпей чаю";
	const std::string greekTextTest = "   Θωθ   \nΟ Θωθ ή και Θωτ ή και Τωθ υπήρξε ένας από τους πλέον δημοφιλείς θεούς της αιγυπτιακής θρησκείας. Ήταν θεότητα της Σελήνης και της Σοφίας. Οι αρχαίοι Έλληνες τον προσδιόρισαν ως τον Ερμή τον Τρισμέγιστο. \n   Ιδιότητες   \nΣτις αρχέγονες περιόδους του αιγυπτιακού πολιτισμού ήταν θεός της Σελήνης και από το σεληνιακό συσχετισμό του λέγεται ότι αντλεί την πολυμορφία του, καθώς εκφράζεται με πολλά και διαφορετικά πρόσωπα. Όπως η Σελήνη αντλεί το φως της από τον Ήλιο, έτσι και ο Θωθ αντλούσε ένα μεγάλο μέρος της εξουσίας του από τον ηλιακό θεό Ρα, όντας γραφέας και σύμβουλός του. Στην πραγματικότητα, τόσο σημαντικές ήταν οι φάσεις της Σελήνης για τους ρυθμούς της αιγυπτιακής ζωής, ώστε ο Θωθ θεωρήθηκε αρχή της κοσμικής τάξης, καθώς και των θρησκευτικών και κοινωνικών ιδρυμάτων. Ήταν παρών σχεδόν σε κάθε όψη λατρείας στους ναούς, στην απονομή δικαιοσύνης και στις μαγικές τέχνες, με τις οποίες σχετιζόταν ιδιαιτέρως.\n\nΕπίσης είναι ο θεϊκός γραφέας, εκείνος που επινόησε τη γραφή και κύριος της σοφίας. Το ιερατείο απέδιδε σ' εκείνον πολλές από τις ιερές γραφές, ανάμεσα στις οποίες συγκαταλέγεται η Βίβλος των Αναπνοών και ένα τμήμα της Βίβλου των Νεκρών. Πιστευόταν άλλωστε ότι είχε μεταδώσει την τέχνη της ιερογλυφικής γραφής στους Αιγυπτίους από τα πανάρχαια χρόνια.[1], ενώ ήταν και προστάτης των γραφέων. Του αποδίδονταν τιμές ως Κύριου της Γνώσης όλων των Επιστημών, θεωρούμενος η προσωποποίηση της Κατανόησης και της Λογικής. Υπήρξε, επίσης, μεσολαβητής για να επέλθει ειρήνη ανάμεσα στον Ώρο και τον Σηθ. Ιδιαίτερο πεδίο δράσης του ήταν η εσωτερική σοφία και γι' αυτό αποκαλείτο «ο Μυστηριώδης» ή «ο Άγνωστος». Οι μαγικές του δυνάμεις τον συνέδεσαν, επίσης, με την ιατρική και, όταν το σώμα υπέκυπτε τελικώς στο θάνατο, εκείνος ήταν πάλι που οδηγούσε το νεκρό στο βασίλειο των θεών και ακολουθούσε η κρίση της ψυχής του.";
    const std::string jpTextTest = "スーパーファミコン（SUPER Famicom）は、任天堂より日本・中華民国（台湾）・香港などで発売された家庭用ゲーム機。略記・略称はSFC、スーファミなど[注 1]。日本発売は1990年（平成2年）11月21日、生産終了は2003年（平成15年）9月30日。\n\nファミリーコンピュータの後継機として開発された。同世代機の中では後発であったが、ファミリーコンピュータに引き続き、最多出荷台数を記録した。\n\n北米・欧州・オーストラリア・ブラジルなどでは“Super Nintendo Entertainment System”（スーパーニンテンドーエンターテインメントシステム、略称：Super NES、またはSNES）の名称で発売された。 ";

    #define textInUse greekTextTest
    std::u32string testString = TextRenderer::preprocess(To_UTF32(textInUse));

    for (int i = 0; i < testString.length() && instruments.size() < 256; i+= 12){
        instruments.push_back(Instrument());
        instruments.back().setName(testString.substr(i, std::min(static_cast<int>(testString.size()-i), 12)));
        instruments.back().setPalette((i / 12)&0x07);
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
            "LIST" chunk of type "pat " - a pattern (the 
            indexes are assumed to be in order). Subchunks:
                "idx " - The pattern indexes themselves,
                16-bit words, size is fixed at 8*2 = 16 bytes.
                "bmaj" - The major beats. Variable size.
                "bmin" - The minor beats. Variable size.
            "note" chunk - a chunk of a "note struct":
                4 bytes - the amount of note structs in 
                this chunk. Each note struct consists of:
                    1 byte - the note value:
                        In range 0..96 for C0..B7, 
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
                        bit 5 - whether a volume value
                        is set,
                        bit 4 - whether any effects are 
                        declared,
                        bit 3 - whether to set this cell as
                        the default cell.
                    1 byte (optional) - Instrument value
                        Only present if bit 6 is set in the
                        flags byte.
                    1 byte (optional) - Volume value
                        Only present if bit 5 is set in the
                        flags byte.
                    X bytes (optional) - Effect data
                        TODO

        
*/

//
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
// in "pat"
const char patternIndexesId [5]     = "idx ";
const char majorBeatsId     [5]     = "bmaj";
const char minorBeatsId     [5]     = "bmin";



// List types
const char infoListType     [5]     = "INFO";
const char songListType     [5]     = "song";
const char patternListType  [5]     = "pat ";



const uint32_t mainBranchVer = 0;
const uint32_t thisBranchVer = 0;

int Project::Load(std::vector<uint8_t>& __data) {
    auto file = RIFF::RIFFReader();
    auto errCode = file.open(__data.data());
    if (errCode) return errCode;
    loadInternal(file);
    // auto errdata = file.readChunkData();
    // if (errdata->errorCode) return errdata->errorCode;
    // auto data = *(errdata->data);
    return 0;
}

int Project::Load(std::ifstream & __file) {
    auto file = RIFF::RIFFReader();
    auto errCode = file.open(&__file);
    if (errCode) return errCode;
    return loadInternal (file);
}

int Project::loadInternal(RIFF::RIFFReader & file) {
    int errCode;
    // 1. Test file type
        if (memcmp(file.rh->h_type, fileType, 4)){
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
        errCode = file.seekNextChunk();

    // And now, read the rest of the file
    while (!errCode) {
        if (!memcmp(file.rh->c_id, listId, 4)){
            // LIST type, has several subtypes
            errCode = file.seekLevelSub();

            auto * type = file.rh->ls[file.rh->ls_level-1].c_type;

            if (!memcmp(type, infoListType, 4)){
                // INFO subchunk
                while (!errCode) {
                    chunkData = file.readChunkData();

                    auto id = file.rh->c_id;

                    if (!memcmp (id, commentsId, 4)){
                        // Comment subsubchunk, gotta convert 'em 
                        // record separator chars into newlines
                        for (auto &c : *chunkData) {
                            if (c == 0x1E) c = 0x0A; // Record Separator -> LineFeed
                        }
                    }

                    printByteArray(chunkData->data(), chunkData->size(), 16);

                    if (chunkData->back() != 0) 
                        chunkData->push_back(0);

                    if (!memcmp (id, nameId, 4))
                        name = std::string((char *)chunkData->data());
                    else if (!memcmp (id, artistId, 4))
                        composer = std::string((char *)chunkData->data());
                    else if (!memcmp (id, copyrightId, 4))
                        copyright = std::string((char *)chunkData->data());
                    else if (!memcmp (id, commentsId, 4))
                        comments = std::string((char *)chunkData->data());
                    else if (!memcmp (id, softwareId, 4)){
                        if ( ! (
                            chunkData->size() == 10 &&
                            !memcmp (chunkData->data(), software, 9)
                        ) )
                            fprintf(stderr, "The \"Software\" field in the Genecyzer file's metadata is not set to \"Genecyzer\". This indicates a file that has been modified by external software, which could lead to invalid file loading.\n");
                    }

                    errCode = file.seekNextChunk();
                }
            }
        } 

        errCode = file.seekNextChunk();
    }

    return errCode == RIFF_ERROR_EOCL ? 0 : errCode;
}

uint8_t * Project::Save(size_t * size_out) {
    auto writer = riff_writerAllocate();

    riff_writer_open_mem(writer);

    riff_writerNewChunk(writer);
    memcpy (writer->c_id, versionId, 5);
    riff_writeInChunk(writer, (void *)thisBranch, 8);
    char buf[4];
    writeBytes(thisBranchVer, buf);
    riff_writeInChunk(writer, buf, 4);
    riff_writerFinishChunk(writer);    

    riff_writerNewChunk(writer);
    memcpy(writer->c_id, effectColumnId, 5);
    riff_writeInChunk(writer, (void *)&effectColumnAmount, 8);
    riff_writerFinishChunk(writer);

    memcpy(writer->h_type, fileType, 5);
    uint8_t * outMem = (uint8_t *)riff_writer_close_mem(writer);
    *size_out = writer->size;
    riff_writerFree(writer);
    return outMem;
}


#endif