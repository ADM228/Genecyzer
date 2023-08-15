#include <cmath>

#include <SFML/Graphics.hpp>


#include "Tile.hpp"
#include "ChrFont.hpp"

#define INCBIN_PREFIX bin_
#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#define INCBIN_SILENCE_BITCODE_WARNING 1    // You cannot imagine how little of a fuck i give
#include "incbin.h"
INCBIN(font, "tilesetUnicode.chr");
#ifdef _MSC_VER
	#include "binIncludes.h"
#endif


int main()
{
    sf::RenderWindow window(sf::VideoMode(200, 200), "Genecyzer");
    sf::View InstrumentView(sf::FloatRect(0.f, 0.f, 200.f, 200.f));
    sf::View TrackerView(sf::FloatRect(0.f,0.f,200.f,200.f));

    unsigned char * fontPointer = const_cast<unsigned char *>(bin_font_data);
    std::vector<uint32_t> codepages = {0x0000, 0x0080, 0x0380, 0x0400, 0x0480, 0x3000, 0x3080};
    ChrFont font(fontPointer, bin_font_size, codepages);

    sf::Sprite sprite0;
    sf::Sprite sprite7;
    int instPage = 0;
    uint8_t scale = 1;
    uint8_t mode = 0;

    

    window.setView(TrackerView);

    sprite0.setTexture(font.texture);
    sprite0.setScale(sf::Vector2f((float)scale, (float)scale));
    sprite0.setTextureRect(sf::IntRect(0, 8*0x30, 8, 8));
    
    //((float)scale, (float)scale));
    //sprite7.setTextureRect(sf::IntRect(0, 8*0x37, 8, 8));


    uint32_t tilesd[] = {0,1,2,2};
    TileMatrix tiles(4,4, 0x30);
    tiles.copyRect(0, 2, 4, 1, tilesd);
    sf::Texture text;
    std::string allTextTest = "English Ελληνικά Русский にほんこ゛ antidisestablishmentarianism\nThe quick brown fox jumped over the lazy dog\nСъешь же ещё этих мягких французских булочек, да выпей чаю";
	std::string greekTextTest = "   Θωθ   \nΟ Θωθ ή και Θωτ ή και Τωθ υπήρξε ένας από τους πλέον δημοφιλείς θεούς της αιγυπτιακής θρησκείας. Ήταν θεότητα της Σελήνης και της Σοφίας. Οι αρχαίοι Έλληνες τον προσδιόρισαν ως τον Ερμή τον Τρισμέγιστο. \n   Ιδιότητες   \nΣτις αρχέγονες περιόδους του αιγυπτιακού πολιτισμού ήταν θεός της Σελήνης και από το σεληνιακό συσχετισμό του λέγεται ότι αντλεί την πολυμορφία του, καθώς εκφράζεται με πολλά και διαφορετικά πρόσωπα. Όπως η Σελήνη αντλεί το φως της από τον Ήλιο, έτσι και ο Θωθ αντλούσε ένα μεγάλο μέρος της εξουσίας του από τον ηλιακό θεό Ρα, όντας γραφέας και σύμβουλός του. Στην πραγματικότητα, τόσο σημαντικές ήταν οι φάσεις της Σελήνης για τους ρυθμούς της αιγυπτιακής ζωής, ώστε ο Θωθ θεωρήθηκε αρχή της κοσμικής τάξης, καθώς και των θρησκευτικών και κοινωνικών ιδρυμάτων. Ήταν παρών σχεδόν σε κάθε όψη λατρείας στους ναούς, στην απονομή δικαιοσύνης και στις μαγικές τέχνες, με τις οποίες σχετιζόταν ιδιαιτέρως.\n\nΕπίσης είναι ο θεϊκός γραφέας, εκείνος που επινόησε τη γραφή και κύριος της σοφίας. Το ιερατείο απέδιδε σ' εκείνον πολλές από τις ιερές γραφές, ανάμεσα στις οποίες συγκαταλέγεται η Βίβλος των Αναπνοών και ένα τμήμα της Βίβλου των Νεκρών. Πιστευόταν άλλωστε ότι είχε μεταδώσει την τέχνη της ιερογλυφικής γραφής στους Αιγυπτίους από τα πανάρχαια χρόνια.[1], ενώ ήταν και προστάτης των γραφέων. Του αποδίδονταν τιμές ως Κύριου της Γνώσης όλων των Επιστημών, θεωρούμενος η προσωποποίηση της Κατανόησης και της Λογικής. Υπήρξε, επίσης, μεσολαβητής για να επέλθει ειρήνη ανάμεσα στον Ώρο και τον Σηθ. Ιδιαίτερο πεδίο δράσης του ήταν η εσωτερική σοφία και γι' αυτό αποκαλείτο «ο Μυστηριώδης» ή «ο Άγνωστος». Οι μαγικές του δυνάμεις τον συνέδεσαν, επίσης, με την ιατρική και, όταν το σώμα υπέκυπτε τελικώς στο θάνατο, εκείνος ήταν πάλι που οδηγούσε το νεκρό στο βασίλειο των θεών και ακολουθούσε η κρίση της ψυχής του.";
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::Resized){
                //TrackerView.reset(sf::FloatRect(0, 0, 32, 32));
                //TrackerView.setViewport(sf::FloatRect(0.f, 0.f, 32.f/event.size.width, 32.f/event.size.height));
				scale = std::max(static_cast<int>(std::ceil(event.size.height/(4*8*8))), 1);
                InstrumentView.reset(sf::FloatRect(instPage*scale*8*16, 0, event.size.width, 64));
                InstrumentView.setViewport(sf::FloatRect(0, 0, scale, 64.f/event.size.height*scale));
                TileMatrix textMatrix = font.renderToTiles(greekTextTest, std::ceil((event.size.width/scale)/8));
                text.create(textMatrix.getWidth()*8+8, textMatrix.getHeight()*8);
				text.update((new TileMatrix(1, textMatrix.getHeight(), 0x20))->renderToTexture(font.texture), textMatrix.getWidth()*8, 0);
                text.update(textMatrix.renderToTexture(font.texture));
                sprite7.setTextureRect(sf::IntRect(0, 0, text.getSize().x, text.getSize().y));
                sprite7.setTexture(text);
            } else if (event.type == sf::Event::KeyPressed){
                if (event.key.code == sf::Keyboard::Left){
                    instPage++;
                    InstrumentView.reset(sf::FloatRect(instPage*scale*8*16, 0, 64, 64));
                }
            }
        }

        window.clear(sf::Color(255,255,0,0));
        window.setView(InstrumentView);

//		TileMatrix instruments

        sprite7.setScale(sf::Vector2f(1.f, 1.f));
        window.draw(sprite7);
        // tiles.render(0,0,&window, font[0]);
        // window.draw(tile.renderVertex, sf::RenderStates(&font[0]));
        // window.draw(tile1.renderVertex, sf::RenderStates(&font[0]));
        // window.draw(tile2.renderVertex, sf::RenderStates(&font[0]));
        // window.draw(tile3.renderVertex, sf::RenderStates(&font[0]));
        // window.setView(TrackerView);
        
        window.display();
    }

    return 0;
}
