#ifndef __PROJECT_INCLUDED__
#define __PROJECT_INCLUDED__

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <fstream>
#include <string>
#include <vector>

#include "TextRenderer.cpp"
#include "Instrument.cpp"
#include "Utils.cpp"
#include "Song.cpp"
#include "riff.hpp"

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

struct ProjectMetadata {
	const std::string & name;
	const std::string & composer;
	const std::string & copyright;
	const std::string & comments;
};

class Project {
    public:
		// Create new default project
		static Project createDefault ();

        // Load project from filestream
        int Load (std::ifstream & file);
        // Load project from memory
        int Load (const std::vector<uint8_t>& data);

        // Save project to filestream
        void Save (std::ofstream & file) const;
        // Save project to memory
        uint8_t * Save (size_t & size_out) const;

        // Export project's patterns to SNESFM opcode format
        uint8_t * exportSNESFM () const;

        // The songs
        std::vector<Song> songs;

        // Global instruments
        std::vector<Instrument> globalInstruments;

        // Set / get methods for the metadata
        void importMetadata (ProjectMetadata & metadata);
        const ProjectMetadata exportMetadata() const;

				std::string&	name()				{return __name;};
		const	std::string&	name()		const	{return __name;};
				std::string&	composer()			{return __composer;};
        const	std::string&	composer()	const	{return __composer;};
				std::string&	copyright()			{return __copyright;};
        const	std::string&	copyright()	const	{return __copyright;};
				std::string&	comments()			{return __comments;};
        const	std::string&	comments()	const	{return __comments;};


    private:

        std::string __name;
        std::string __composer;
        std::string __copyright;
        std::string __comments;


};

Project Project::createDefault() {
    Project project;
	project.songs.push_back(Song::createDefault());
    static const std::string allTextTest = "English Ελληνικά Русский にほんこご antidisestablishmentarianism\nThe quick brown fox jumped over the lazy dog\nСъешь же ещё этих мягких французских булочек, да выпей чаю";
	static const std::string greekTextTest = "   Θωθ   \nΟ Θωθ ή και Θωτ ή και Τωθ υπήρξε ένας από τους πλέον δημοφιλείς θεούς της αιγυπτιακής θρησκείας. Ήταν θεότητα της Σελήνης και της Σοφίας. Οι αρχαίοι Έλληνες τον προσδιόρισαν ως τον Ερμή τον Τρισμέγιστο. \n   Ιδιότητες   \nΣτις αρχέγονες περιόδους του αιγυπτιακού πολιτισμού ήταν θεός της Σελήνης και από το σεληνιακό συσχετισμό του λέγεται ότι αντλεί την πολυμορφία του, καθώς εκφράζεται με πολλά και διαφορετικά πρόσωπα. Όπως η Σελήνη αντλεί το φως της από τον Ήλιο, έτσι και ο Θωθ αντλούσε ένα μεγάλο μέρος της εξουσίας του από τον ηλιακό θεό Ρα, όντας γραφέας και σύμβουλός του. Στην πραγματικότητα, τόσο σημαντικές ήταν οι φάσεις της Σελήνης για τους ρυθμούς της αιγυπτιακής ζωής, ώστε ο Θωθ θεωρήθηκε αρχή της κοσμικής τάξης, καθώς και των θρησκευτικών και κοινωνικών ιδρυμάτων. Ήταν παρών σχεδόν σε κάθε όψη λατρείας στους ναούς, στην απονομή δικαιοσύνης και στις μαγικές τέχνες, με τις οποίες σχετιζόταν ιδιαιτέρως.\n\nΕπίσης είναι ο θεϊκός γραφέας, εκείνος που επινόησε τη γραφή και κύριος της σοφίας. Το ιερατείο απέδιδε σ' εκείνον πολλές από τις ιερές γραφές, ανάμεσα στις οποίες συγκαταλέγεται η Βίβλος των Αναπνοών και ένα τμήμα της Βίβλου των Νεκρών. Πιστευόταν άλλωστε ότι είχε μεταδώσει την τέχνη της ιερογλυφικής γραφής στους Αιγυπτίους από τα πανάρχαια χρόνια.[1], ενώ ήταν και προστάτης των γραφέων. Του αποδίδονταν τιμές ως Κύριου της Γνώσης όλων των Επιστημών, θεωρούμενος η προσωποποίηση της Κατανόησης και της Λογικής. Υπήρξε, επίσης, μεσολαβητής για να επέλθει ειρήνη ανάμεσα στον Ώρο και τον Σηθ. Ιδιαίτερο πεδίο δράσης του ήταν η εσωτερική σοφία και γι' αυτό αποκαλείτο «ο Μυστηριώδης» ή «ο Άγνωστος». Οι μαγικές του δυνάμεις τον συνέδεσαν, επίσης, με την ιατρική και, όταν το σώμα υπέκυπτε τελικώς στο θάνατο, εκείνος ήταν πάλι που οδηγούσε το νεκρό στο βασίλειο των θεών και ακολουθούσε η κρίση της ψυχής του.";
    static const std::string jpTextTest = "スーパーファミコン（SUPER Famicom）は、任天堂より日本・中華民国（台湾）・香港などで発売された家庭用ゲーム機。略記・略称はSFC、スーファミなど[注 1]。日本発売は1990年（平成2年）11月21日、生産終了は2003年（平成15年）9月30日。\n\nファミリーコンピュータの後継機として開発された。同世代機の中では後発であったが、ファミリーコンピュータに引き続き、最多出荷台数を記録した。\n\n北米・欧州・オーストラリア・ブラジルなどでは“Super Nintendo Entertainment System”（スーパーニンテンドーエンターテインメントシステム、略称：Super NES、またはSNES）の名称で発売された。 ";

    #define textInUse greekTextTest
    std::u32string testString = TextRenderer::preprocess(To_UTF32(textInUse));

    for (int i = 0; i < testString.length() && project.globalInstruments.size() < 256; i+= 12){
        project.globalInstruments.push_back(Instrument());
        project.globalInstruments.back().setName(testString.substr(i, std::min(static_cast<int>(testString.size()-i), 12)));
        project.globalInstruments.back().setPalette((i / 12)&0x07);
    }
    return project;
}

int Project::Load(const std::vector<uint8_t>& __data) {
    auto file = RIFF::RIFFReader();
    auto errCode = file.open(__data.data());
    if (errCode) return errCode;
    // loadInternal(file);
    // auto errdata = file.readChunkData();
    // if (errdata->errorCode) return errdata->errorCode;
    // auto data = *(errdata->data);
    return 0;
}

int Project::Load(std::ifstream & __file) {
    auto file = RIFF::RIFFReader();
    auto errCode = file.open(__file);
    if (errCode) return errCode;
    // return loadInternal (file);
    return 0;
}

void Project::Save (std::ofstream & file) const{

}

uint8_t * Project::Save(size_t & size_out) const{
    RIFF::RIFFWriter writer;

    writer.openMem();

    // saveInternal(writer);

    writer.close();
    size_out = writer().size;
    uint8_t * outMem = (uint8_t *)writer.file;
    return outMem;
}

const ProjectMetadata Project::exportMetadata() const{
	return ProjectMetadata {
		__name, __composer, __copyright, __comments
	};
}

#endif