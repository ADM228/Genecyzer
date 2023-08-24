#include <cmath>
#include <cstdio>

#include <SFML/Graphics.hpp>


#include "Tile.hpp"
#include "ChrFont.hpp"
#include "Instrument.hpp"
#include "StrConvert.hpp"

#include "binIncludes.cpp"

uint8_t instSelected = 0;
uint8_t scale = 1;
uint8_t mode = 0;

int debug = 0;

std::vector<Instrument> instruments;
sf::Sprite instrumentSprite;


#pragma region Update
bool forceUpdateAll = 0;

uint64_t updateSections = 0;
constexpr uint64_t UPDATE_SCALE = 1;
constexpr uint64_t UPDATE_INST_POS = 2;
constexpr uint64_t UPDATE_INST_lIST = 4;

std::vector<uint8_t> instrumentsToUpdate;
#pragma endregion

#pragma region ForwardDeclarations
void renderInstList(sf::RenderWindow *, sf::View *, ChrFont *, sf::Texture *);
void updateInstPage(sf::RenderWindow *, sf::View *);
#pragma endregion

int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "Genecyzer");
    sf::View InstrumentView(sf::FloatRect(0.f, 0.f, 200.f, 200.f));
    sf::View TrackerView(sf::FloatRect(0.f,0.f,200.f,200.f));

    unsigned char * fontPointer = const_cast<unsigned char *>(bin_font_data);
    std::vector<uint32_t> codepages = {0x0000, 0x0080, 0x0380, 0x0400, 0x0480, 0x3000, 0x3080};
    ChrFont font(fontPointer, bin_font_size, codepages, 1);

    sf::Sprite sprite0;

    sf::Texture instrumentTexture;
    

    window.setView(TrackerView);

    sprite0.setTexture(font.texture);
    sprite0.setScale(sf::Vector2f((float)scale, (float)scale));
    sprite0.setTextureRect(sf::IntRect(0, 8*0x30, 8, 8));

    uint32_t tilesd[] = {0,1,2,2};
    TileMatrix tiles(4,4, 0x30);
    tiles.copyRect(0, 2, 4, 1, tilesd);
    std::string allTextTest = "English Ελληνικά Русский にほんごご antidisestablishmentarianism\nThe quick brown fox jumped over the lazy dog\nСъешь же ещё этих мягких французских булочек, да выпей чаю";
	std::string greekTextTest = "   Θωθ   \nΟ Θωθ ή και Θωτ ή και Τωθ υπήρξε ένας από τους πλέον δημοφιλείς θεούς της αιγυπτιακής θρησκείας. Ήταν θεότητα της Σελήνης και της Σοφίας. Οι αρχαίοι Έλληνες τον προσδιόρισαν ως τον Ερμή τον Τρισμέγιστο. \n   Ιδιότητες   \nΣτις αρχέγονες περιόδους του αιγυπτιακού πολιτισμού ήταν θεός της Σελήνης και από το σεληνιακό συσχετισμό του λέγεται ότι αντλεί την πολυμορφία του, καθώς εκφράζεται με πολλά και διαφορετικά πρόσωπα. Όπως η Σελήνη αντλεί το φως της από τον Ήλιο, έτσι και ο Θωθ αντλούσε ένα μεγάλο μέρος της εξουσίας του από τον ηλιακό θεό Ρα, όντας γραφέας και σύμβουλός του. Στην πραγματικότητα, τόσο σημαντικές ήταν οι φάσεις της Σελήνης για τους ρυθμούς της αιγυπτιακής ζωής, ώστε ο Θωθ θεωρήθηκε αρχή της κοσμικής τάξης, καθώς και των θρησκευτικών και κοινωνικών ιδρυμάτων. Ήταν παρών σχεδόν σε κάθε όψη λατρείας στους ναούς, στην απονομή δικαιοσύνης και στις μαγικές τέχνες, με τις οποίες σχετιζόταν ιδιαιτέρως.\n\nΕπίσης είναι ο θεϊκός γραφέας, εκείνος που επινόησε τη γραφή και κύριος της σοφίας. Το ιερατείο απέδιδε σ' εκείνον πολλές από τις ιερές γραφές, ανάμεσα στις οποίες συγκαταλέγεται η Βίβλος των Αναπνοών και ένα τμήμα της Βίβλου των Νεκρών. Πιστευόταν άλλωστε ότι είχε μεταδώσει την τέχνη της ιερογλυφικής γραφής στους Αιγυπτίους από τα πανάρχαια χρόνια.[1], ενώ ήταν και προστάτης των γραφέων. Του αποδίδονταν τιμές ως Κύριου της Γνώσης όλων των Επιστημών, θεωρούμενος η προσωποποίηση της Κατανόησης και της Λογικής. Υπήρξε, επίσης, μεσολαβητής για να επέλθει ειρήνη ανάμεσα στον Ώρο και τον Σηθ. Ιδιαίτερο πεδίο δράσης του ήταν η εσωτερική σοφία και γι' αυτό αποκαλείτο «ο Μυστηριώδης» ή «ο Άγνωστος». Οι μαγικές του δυνάμεις τον συνέδεσαν, επίσης, με την ιατρική και, όταν το σώμα υπέκυπτε τελικώς στο θάνατο, εκείνος ήταν πάλι που οδηγούσε το νεκρό στο βασίλειο των θεών και ακολουθούσε η κρίση της ψυχής του.";
    std::string jpTextTest = "スーパーファミコン（SUPER Famicom）は、任天堂より日本・中華民国（台湾）・香港などで発売された家庭用ゲーム機。略記・略称はSFC、スーファミなど[注 1]。日本発売は1990年（平成2年）11月21日、生産終了は2003年（平成15年）9月30日。\n\nファミリーコンピュータの後継機として開発された。同世代機の中では後発であったが、ファミリーコンピュータに引き続き、最多出荷台数を記録した。\n\n北米・欧州・オーストラリア・ブラジルなどでは“Super Nintendo Entertainment System”（スーパーニンテンドーエンターテインメントシステム、略称：Super NES、またはSNES）の名称で発売された。 ";

    forceUpdateAll = 1;

    #define t greekTextTest

    std::u32string currentTestText = To_UTF32(t);

    for (int i = 0; i < currentTestText.length() && instruments.size() < 256; i+= 12){
        instruments.push_back(Instrument());
        instruments.back().setName(currentTestText.substr(i, std::min(static_cast<int>(currentTestText.size()-i), 12)));
        instruments.back().setPalette((i / 12)&0x07);
    }

    while (window.isOpen())
    {
        #pragma region EventHandling

        sf::Event event;

        updateSections = 0;

        instrumentsToUpdate.clear();

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized){
                //TrackerView.reset(sf::FloatRect(0, 0, 32, 32));
                //TrackerView.setViewport(sf::FloatRect(0.f, 0.f, 32.f/event.size.width, 32.f/event.size.height));
				scale = std::max(static_cast<int>(std::ceil(event.size.height/(4*8*8))), 1);
                updateSections |= UPDATE_SCALE;

                //TileMatrix textMatrix = font.renderToTiles(currentTestText, std::ceil((event.size.width/scale)/8));

                //int width = std::ceil((event.size.width/scale)/8);


            } else if (event.type == sf::Event::KeyPressed){
                if (event.key.code == sf::Keyboard::Right){
                    if (instSelected < 256-8)
                        instrumentsToUpdate.push_back(instSelected + 8);
                    else
                        instrumentsToUpdate.push_back(255);
                    
                    if (instrumentsToUpdate.back() != instSelected){
                        instrumentsToUpdate.push_back(instSelected);
                        instSelected = instrumentsToUpdate[instrumentsToUpdate.size()-2] & 0xFF;
                        updateSections |= UPDATE_INST_POS;
                    } else
                        instrumentsToUpdate.pop_back();
                        
                } else if (event.key.code == sf::Keyboard::Left){
                    if (instSelected >= 8)
                        instrumentsToUpdate.push_back(instSelected - 8);
                    else
                        instrumentsToUpdate.push_back(0);
                    
                    if (instrumentsToUpdate.back() != instSelected){
                        instrumentsToUpdate.push_back(instSelected);
                        instSelected = instrumentsToUpdate[instrumentsToUpdate.size()-2] & 0xFF;
                        updateSections |= UPDATE_INST_POS;
                    } else
                        instrumentsToUpdate.pop_back();
                } else if (event.key.code == sf::Keyboard::Up){
                    if (instSelected > 0)
                        instrumentsToUpdate.push_back(instSelected - 1);
                    else
                        instrumentsToUpdate.push_back(0);
                    
                    if (instrumentsToUpdate.back() != instSelected){
                        instrumentsToUpdate.push_back(instSelected);
                        instSelected = instrumentsToUpdate[instrumentsToUpdate.size()-2] & 0xFF;
                        updateSections |= UPDATE_INST_POS;
                    } else
                        instrumentsToUpdate.pop_back();
                } else if (event.key.code == sf::Keyboard::Down){
                    if (instSelected < 255)
                        instrumentsToUpdate.push_back(instSelected + 1);
                    else
                        instrumentsToUpdate.push_back(255);
                    
                    if (instrumentsToUpdate.back() != instSelected){
                        instrumentsToUpdate.push_back(instSelected);
                        instSelected = instrumentsToUpdate[instrumentsToUpdate.size()-2] & 0xFF;
                        updateSections |= UPDATE_INST_POS;
                    } else
                        instrumentsToUpdate.pop_back();
                }
            }
        }

        #pragma endregion
        #pragma region ConditionalUpdates

        if (forceUpdateAll){
            updateSections = 0xFFFFFFFFFFFFFFFF;
            instrumentsToUpdate.clear();
            forceUpdateAll = 0;
        }

        if (updateSections & UPDATE_INST_POS)
            renderInstList(&window, &InstrumentView, &font, &instrumentTexture);

        if (updateSections & (UPDATE_INST_POS | UPDATE_SCALE))          
            updateInstPage(&window, &InstrumentView);

        #pragma endregion
        #pragma region AlwaysUpdates


        window.setView(InstrumentView);
        window.clear(sf::Color(255,255,0,0));


        window.draw(instrumentSprite);
        // tiles.render(0,0,&window, font[0]);
        // window.draw(tile.renderVertex, sf::RenderStates(&font[0]));
        // window.draw(tile1.renderVertex, sf::RenderStates(&font[0]));
        // window.draw(tile2.renderVertex, sf::RenderStates(&font[0]));
        // window.draw(tile3.renderVertex, sf::RenderStates(&font[0]));
        // window.setView(TrackerView);
        
        window.display();
        #pragma endregion

    }

    return 0;
}

void renderInstList (sf::RenderWindow *window, sf::View *view, ChrFont *font, sf::Texture *instrumentTexture) {
    constexpr uint16_t INST_WIDTH = 512;
    constexpr uint16_t INST_HEIGHT = 8;


    if (instrumentsToUpdate.size() == 0){   // Update the entire list


        TileMatrix instrumentMatrix = TileMatrix(INST_WIDTH, INST_HEIGHT, 0x20);
        uint8_t instNumber;
        uint8_t palette;

        for (int i = 0; i < INST_WIDTH; i+=16){
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
                TileMatrix string = font->renderToTiles(output, 15);
                string.resize(16, 1);
                string.fillInvert(instNumber == instSelected);
                string.fillPaletteRect(0, 0, 16, 1, palette);
                instrumentMatrix.copyRect(i, j, 16, 1, &string, 0, 0);
                instNumber++;
            }
        }

        instrumentTexture->create(INST_WIDTH*8, INST_HEIGHT*8);
        // instrumentTexture->update((new TileMatrix(1, INST_HEIGHT, 0x20))->renderToTexture(font->texture), INST_WIDTH*8, 0);
        instrumentTexture->update(instrumentMatrix.renderToTexture(font->texture));

        instrumentSprite.setTextureRect(sf::IntRect(0, 0, instrumentTexture->getSize().x, instrumentTexture->getSize().y));
        instrumentSprite.setTexture(*instrumentTexture);
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
            TileMatrix string = font->renderToTiles(output, 15);
            string.resize(16, 1);
            string.fillInvert(instNumber == instSelected);
            string.fillPaletteRect(0, 0, 16, 1, palette);
            instrumentTexture->update(string.renderToTexture(font->texture), (instNumber&0xF8)<<(1+3), (instNumber&0x07)<<3);
        }
    }
}

void updateInstPage (sf::RenderWindow *window, sf::View *view) {
    if (((instSelected&0xF8)+8/2)*16*scale*2 < window->getSize().x && window->getSize().x >= 8*16*scale)
        // = (IS>>3)*8*16*scale < winWidth/2 - 8*16*scale/2
        // Align left
        view->reset(sf::FloatRect(0, 0, window->getSize().x, 64));
    else if ((32*8-(instSelected&0xF8)-8/2)*16*scale*2 < window->getSize().x && window->getSize().x >= 8*16*scale)
        // = (32-IS>>3)*8*16*scale < winWidth/2 + 8*16*scale/2
        // Align right
        view->reset(sf::FloatRect(32*8*16-(window->getSize().x/scale), 0, window->getSize().x, 64));
    else
        view->reset(sf::FloatRect(((instSelected>>3)+1)*8*16-(window->getSize().x/(scale*2))-8*8, 0, window->getSize().x, 64));
    view->setViewport(sf::FloatRect(0, 0, scale, 64.f/window->getSize().y*scale));
}
