#ifndef __INSTANCE_INCLUDED__
#define __INSTANCE_INCLUDED__

#include <SFML/Graphics.hpp>

#include "Tile.cpp"
#include "ChrFont.cpp"
#include "Instrument.cpp"
#include "StrConvert.cpp"
#include "TextRenderer.cpp"

#include <cmath>

constexpr uint64_t UPDATE_SCALE = 1;
constexpr uint64_t UPDATE_INST_POS = 2;
constexpr uint64_t UPDATE_INST_lIST = 4;

#define TILE_SIZE 8
#define INST_ENTRY_WIDTH 16

class Instance {

    public:
        Instance();
        void ProcessEvents();
        void Update();

        void addMonospaceFont(const uint8_t* data, uint32_t size, std::vector<uint32_t> codepages);


        bool isWindowOpen(){ return window.isOpen(); };

    protected:
        void eventHandleInstList (int, int, uint8_t, bool);
        void renderInstList();
        void updateInstPage();

    private:
        uint8_t instSelected = 0;
        uint8_t scale = 1;
        uint8_t mode = 0;

        int debug = 0;

        std::vector<Instrument> instruments;
        sf::Sprite instrumentSprite;


        #pragma region Update
        bool forceUpdateAll = 0;

        uint64_t updateSections = 0;

        std::vector<uint8_t> instrumentsToUpdate;
        #pragma endregion

        sf::RenderWindow window;
        sf::View InstrumentView;
        sf::View TrackerView;

        ChrFont font;

        sf::Texture instrumentTexture;
        TileMatrix trackerMatrix;

        std::u32string testString;

};

Instance::Instance() {
    window.create(sf::VideoMode(200, 200), "Genecyzer");
    window.setFramerateLimit(60);
    InstrumentView.reset(sf::FloatRect(0.f, 0.f, 200.f, 200.f));
    TrackerView.reset(sf::FloatRect(0.f,0.f,200.f,200.f));
    forceUpdateAll = 1;


    std::string allTextTest = "English Ελληνικά Русский にほんこご antidisestablishmentarianism\nThe quick brown fox jumped over the lazy dog\nСъешь же ещё этих мягких французских булочек, да выпей чаю";
	std::string greekTextTest = "   Θωθ   \nΟ Θωθ ή και Θωτ ή και Τωθ υπήρξε ένας από τους πλέον δημοφιλείς θεούς της αιγυπτιακής θρησκείας. Ήταν θεότητα της Σελήνης και της Σοφίας. Οι αρχαίοι Έλληνες τον προσδιόρισαν ως τον Ερμή τον Τρισμέγιστο. \n   Ιδιότητες   \nΣτις αρχέγονες περιόδους του αιγυπτιακού πολιτισμού ήταν θεός της Σελήνης και από το σεληνιακό συσχετισμό του λέγεται ότι αντλεί την πολυμορφία του, καθώς εκφράζεται με πολλά και διαφορετικά πρόσωπα. Όπως η Σελήνη αντλεί το φως της από τον Ήλιο, έτσι και ο Θωθ αντλούσε ένα μεγάλο μέρος της εξουσίας του από τον ηλιακό θεό Ρα, όντας γραφέας και σύμβουλός του. Στην πραγματικότητα, τόσο σημαντικές ήταν οι φάσεις της Σελήνης για τους ρυθμούς της αιγυπτιακής ζωής, ώστε ο Θωθ θεωρήθηκε αρχή της κοσμικής τάξης, καθώς και των θρησκευτικών και κοινωνικών ιδρυμάτων. Ήταν παρών σχεδόν σε κάθε όψη λατρείας στους ναούς, στην απονομή δικαιοσύνης και στις μαγικές τέχνες, με τις οποίες σχετιζόταν ιδιαιτέρως.\n\nΕπίσης είναι ο θεϊκός γραφέας, εκείνος που επινόησε τη γραφή και κύριος της σοφίας. Το ιερατείο απέδιδε σ' εκείνον πολλές από τις ιερές γραφές, ανάμεσα στις οποίες συγκαταλέγεται η Βίβλος των Αναπνοών και ένα τμήμα της Βίβλου των Νεκρών. Πιστευόταν άλλωστε ότι είχε μεταδώσει την τέχνη της ιερογλυφικής γραφής στους Αιγυπτίους από τα πανάρχαια χρόνια.[1], ενώ ήταν και προστάτης των γραφέων. Του αποδίδονταν τιμές ως Κύριου της Γνώσης όλων των Επιστημών, θεωρούμενος η προσωποποίηση της Κατανόησης και της Λογικής. Υπήρξε, επίσης, μεσολαβητής για να επέλθει ειρήνη ανάμεσα στον Ώρο και τον Σηθ. Ιδιαίτερο πεδίο δράσης του ήταν η εσωτερική σοφία και γι' αυτό αποκαλείτο «ο Μυστηριώδης» ή «ο Άγνωστος». Οι μαγικές του δυνάμεις τον συνέδεσαν, επίσης, με την ιατρική και, όταν το σώμα υπέκυπτε τελικώς στο θάνατο, εκείνος ήταν πάλι που οδηγούσε το νεκρό στο βασίλειο των θεών και ακολουθούσε η κρίση της ψυχής του.";
    std::string jpTextTest = "スーパーファミコン（SUPER Famicom）は、任天堂より日本・中華民国（台湾）・香港などで発売された家庭用ゲーム機。略記・略称はSFC、スーファミなど[注 1]。日本発売は1990年（平成2年）11月21日、生産終了は2003年（平成15年）9月30日。\n\nファミリーコンピュータの後継機として開発された。同世代機の中では後発であったが、ファミリーコンピュータに引き続き、最多出荷台数を記録した。\n\n北米・欧州・オーストラリア・ブラジルなどでは“Super Nintendo Entertainment System”（スーパーニンテンドーエンターテインメントシステム、略称：Super NES、またはSNES）の名称で発売された。 ";

    forceUpdateAll = 1;

    #define t jpTextTest
    testString = TextRenderer::preprocess(To_UTF32(t));

    for (int i = 0; i < testString.length() && instruments.size() < 256; i+= 12){
        instruments.push_back(Instrument());
        instruments.back().setName(testString.substr(i, std::min(static_cast<int>(testString.size()-i), 12)));
        instruments.back().setPalette((i / 12)&0x07);
    }



}

void Instance::addMonospaceFont(const uint8_t *data, uint32_t size, std::vector<uint32_t> codepages){
    unsigned char * fontPointer = const_cast<unsigned char *>(data);
    font.init(fontPointer, size, codepages, 1);
}

void Instance::ProcessEvents(){



    sf::Event event;

    updateSections = 0;

    instrumentsToUpdate.clear();

    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window.close();
        else if (event.type == sf::Event::Resized){
            scale = std::max(static_cast<int>(std::ceil(event.size.height/(4*8*TILE_SIZE))), 1);
            TrackerView.reset(sf::FloatRect(0, 0, event.size.width, event.size.height/scale));
            TrackerView.setViewport(sf::FloatRect(0.f, 64*scale/(double)event.size.height, scale, 1));
            trackerMatrix = TextRenderer::render(testString, &font, std::ceil((event.size.width/scale)/TILE_SIZE), false, false);
            trackerMatrix.resize(trackerMatrix.getWidth()+1, trackerMatrix.getHeight(), 0x20);
            updateSections |= UPDATE_SCALE;

            //int width = std::ceil((event.size.width/scale)/TILE_SIZE);


        } else if (event.type == sf::Event::KeyPressed){
            
            if (event.key.code == sf::Keyboard::Down)
                eventHandleInstList (255, +1, 255, false);
            else if (event.key.code == sf::Keyboard::Up)
                eventHandleInstList (0, -1, 0, true);
            else if (event.key.code == sf::Keyboard::Right)
                eventHandleInstList (256-8, +8, 255, false);
            else if (event.key.code == sf::Keyboard::Left)
                eventHandleInstList (7, -8, 0, true);
        }
    }
}

void Instance::Update(){
        
    #pragma region ConditionalUpdates

    if (forceUpdateAll){
        updateSections = 0xFFFFFFFFFFFFFFFF;
        instrumentsToUpdate.clear();
        forceUpdateAll = 0;
    }

    if (updateSections & UPDATE_INST_POS)
        renderInstList();

    if (updateSections & (UPDATE_INST_POS | UPDATE_SCALE))          
        updateInstPage();

    #pragma endregion
    #pragma region AlwaysUpdates

    window.clear(sf::Color(255,255,0,0));

    window.setView(InstrumentView);
    window.draw(instrumentSprite);



    window.setView(TrackerView);
    trackerMatrix.render(0, 64, &window, font.texture);

    // tiles.render(0,0,&window, font[0]);
    // window.draw(tile.renderVertex, sf::RenderStates(&font[0]));
    // window.draw(tile1.renderVertex, sf::RenderStates(&font[0]));
    // window.draw(tile2.renderVertex, sf::RenderStates(&font[0]));
    // window.draw(tile3.renderVertex, sf::RenderStates(&font[0]));
    // window.setView(TrackerView);
    
    window.display();
    #pragma endregion

}

void Instance::eventHandleInstList (int limit, int modifier, uint8_t replacement, bool more){
    if ((!more && instSelected < limit) || (more && instSelected > limit))
        instrumentsToUpdate.push_back(instSelected + modifier);
    else
        instrumentsToUpdate.push_back(replacement);
    
    if (instrumentsToUpdate.back() != instSelected){
        instrumentsToUpdate.push_back(instSelected);
        instSelected = instrumentsToUpdate[instrumentsToUpdate.size()-2] & 0xFF;
        updateSections |= UPDATE_INST_POS;
    } else
        instrumentsToUpdate.pop_back();
}

void Instance::renderInstList () {
    #define INST_WIDTH  512
    #define INST_HEIGHT 8


    if (instrumentsToUpdate.size() == 0){   // Update the entire list


        TileMatrix instrumentMatrix = TileMatrix(INST_WIDTH, INST_HEIGHT, 0x20);
        uint8_t instNumber;
        uint8_t palette;

        for (int i = 0; i < INST_WIDTH; i+=INST_ENTRY_WIDTH){
            instNumber = i>>1;
            for (int j = 0; j < INST_HEIGHT; j++){
                std::string output;
                if (instNumber < instruments.size()){
                    char numchar[5];
                    std::snprintf(numchar, 5, "%02X:", instNumber);
                    std::string num(numchar);
                    output = num + instruments[instNumber].getName() + " ";
                    palette = instruments[instNumber].getPalette();
                    if (palette == 0) palette = 7;
                } else { 
                    char numchar[17];
                    std::snprintf(numchar, 17, "%02X:             ", instNumber);
                    std::string num(numchar);
                    output = num;
                    palette = 7;                   
                }
                TileMatrix string = TextRenderer::render(To_UTF32(output), &font, 15);
                string.resize(INST_ENTRY_WIDTH, 1);
                string.fillInvert(instNumber == instSelected);
                string.fillPaletteRect(0, 0, INST_ENTRY_WIDTH, 1, palette);
                instrumentMatrix.copyRect(i, j, INST_ENTRY_WIDTH, 1, &string, 0, 0);
                instNumber++;
            }
        }

        instrumentTexture.create(INST_WIDTH*TILE_SIZE, INST_HEIGHT*TILE_SIZE);
        // instrumentTexture->update((new TileMatrix(1, INST_HEIGHT, 0x20))->renderToTexture(font->texture), INST_WIDTH*TILE_SIZE, 0);
        instrumentTexture.update(instrumentMatrix.renderToTexture(font.texture));

        instrumentSprite.setTextureRect(sf::IntRect(0, 0, instrumentTexture.getSize().x, instrumentTexture.getSize().y));
        instrumentSprite.setTexture(instrumentTexture);
    } else {    // Only update certain instruments
        while (instrumentsToUpdate.size() > 0){
            uint8_t instNumber = instrumentsToUpdate.back();
            instrumentsToUpdate.pop_back();
            uint8_t palette;
            std::string output;
            if (instNumber < instruments.size()){
                char numchar[5];
                std::snprintf(numchar, 5, "%02X:", instNumber);
                std::string num(numchar);
                output = num + instruments[instNumber].getName() + " ";
                palette = instruments[instNumber].getPalette();
                if (palette == 0) palette = 7;
            } else { 
                char numchar[17];
                std::snprintf(numchar, 17, "%02X:             ", instNumber);
                std::string num(numchar);
                output = num;
                palette = 7;                   
            }
            TileMatrix string = TextRenderer::render(To_UTF32(output), &font, 15);
            string.resize(INST_ENTRY_WIDTH, 1);
            string.fillInvert(instNumber == instSelected);
            string.fillPaletteRect(0, 0, INST_ENTRY_WIDTH, 1, palette);
            instrumentTexture.update(string.renderToTexture(font.texture), (instNumber&0xF8)<<(1+3), (instNumber&0x07)<<3);
        }
    }
}

void Instance::updateInstPage () {
    if (((instSelected&0xF8)+TILE_SIZE/2)*INST_ENTRY_WIDTH*scale*2 < window.getSize().x && window.getSize().x >= TILE_SIZE*INST_ENTRY_WIDTH*scale)
        // = (IS>>3)*TILE_SIZE*INST_ENTRY_WIDTH*scale < winWidth/2 - TILE_SIZE*INST_ENTRY_WIDTH*scale/2
        // Align left
        InstrumentView.reset(sf::FloatRect(0, 0, window.getSize().x, 64));
    else if ((32*TILE_SIZE-(instSelected&0xF8)-TILE_SIZE/2)*INST_ENTRY_WIDTH*scale*2 < window.getSize().x && window.getSize().x >= TILE_SIZE*INST_ENTRY_WIDTH*scale)
        // = (32-IS>>3)*TILE_SIZE*INST_ENTRY_WIDTH*scale < winWidth/2 + TILE_SIZE*INST_ENTRY_WIDTH*scale/2
        // Align right
        InstrumentView.reset(sf::FloatRect(32*TILE_SIZE*INST_ENTRY_WIDTH-(window.getSize().x/scale), 0, window.getSize().x, 64));
    else
        InstrumentView.reset(sf::FloatRect(((instSelected>>3)+1)*TILE_SIZE*INST_ENTRY_WIDTH-(window.getSize().x/(scale*2))-TILE_SIZE*8, 0, window.getSize().x, 64));
    InstrumentView.setViewport(sf::FloatRect(0, 0, scale, 64.f/window.getSize().y*scale));
}

#endif // __INSTANCE_INCLUDED__
