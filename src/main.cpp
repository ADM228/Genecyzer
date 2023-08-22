#include <cmath>
#include <cstdio>

#include <SFML/Graphics.hpp>


#include "Tile.hpp"
#include "ChrFont.hpp"
#include "Instrument.hpp"
#include "StrConvert.hpp"

#include "binIncludes.cpp"

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

    sf::Texture text;
    uint32_t tilesd[] = {0,1,2,2};
    TileMatrix tiles(4,4, 0x30);
    tiles.copyRect(0, 2, 4, 1, tilesd);
    std::string allTextTest = "English Ελληνικά Русский にほんごご antidisestablishmentarianism\nThe quick brown fox jumped over the lazy dog\nСъешь же ещё этих мягких французских булочек, да выпей чаю";
	std::string greekTextTest = "   Θωθ   \nΟ Θωθ ή και Θωτ ή και Τωθ υπήρξε ένας από τους πλέον δημοφιλείς θεούς της αιγυπτιακής θρησκείας. Ήταν θεότητα της Σελήνης και της Σοφίας. Οι αρχαίοι Έλληνες τον προσδιόρισαν ως τον Ερμή τον Τρισμέγιστο. \n   Ιδιότητες   \nΣτις αρχέγονες περιόδους του αιγυπτιακού πολιτισμού ήταν θεός της Σελήνης και από το σεληνιακό συσχετισμό του λέγεται ότι αντλεί την πολυμορφία του, καθώς εκφράζεται με πολλά και διαφορετικά πρόσωπα. Όπως η Σελήνη αντλεί το φως της από τον Ήλιο, έτσι και ο Θωθ αντλούσε ένα μεγάλο μέρος της εξουσίας του από τον ηλιακό θεό Ρα, όντας γραφέας και σύμβουλός του. Στην πραγματικότητα, τόσο σημαντικές ήταν οι φάσεις της Σελήνης για τους ρυθμούς της αιγυπτιακής ζωής, ώστε ο Θωθ θεωρήθηκε αρχή της κοσμικής τάξης, καθώς και των θρησκευτικών και κοινωνικών ιδρυμάτων. Ήταν παρών σχεδόν σε κάθε όψη λατρείας στους ναούς, στην απονομή δικαιοσύνης και στις μαγικές τέχνες, με τις οποίες σχετιζόταν ιδιαιτέρως.\n\nΕπίσης είναι ο θεϊκός γραφέας, εκείνος που επινόησε τη γραφή και κύριος της σοφίας. Το ιερατείο απέδιδε σ' εκείνον πολλές από τις ιερές γραφές, ανάμεσα στις οποίες συγκαταλέγεται η Βίβλος των Αναπνοών και ένα τμήμα της Βίβλου των Νεκρών. Πιστευόταν άλλωστε ότι είχε μεταδώσει την τέχνη της ιερογλυφικής γραφής στους Αιγυπτίους από τα πανάρχαια χρόνια.[1], ενώ ήταν και προστάτης των γραφέων. Του αποδίδονταν τιμές ως Κύριου της Γνώσης όλων των Επιστημών, θεωρούμενος η προσωποποίηση της Κατανόησης και της Λογικής. Υπήρξε, επίσης, μεσολαβητής για να επέλθει ειρήνη ανάμεσα στον Ώρο και τον Σηθ. Ιδιαίτερο πεδίο δράσης του ήταν η εσωτερική σοφία και γι' αυτό αποκαλείτο «ο Μυστηριώδης» ή «ο Άγνωστος». Οι μαγικές του δυνάμεις τον συνέδεσαν, επίσης, με την ιατρική και, όταν το σώμα υπέκυπτε τελικώς στο θάνατο, εκείνος ήταν πάλι που οδηγούσε το νεκρό στο βασίλειο των θεών και ακολουθούσε η κρίση της ψυχής του.";
    std::string jpTextTest = "スーパーファミコン（SUPER Famicom）は、任天堂より日本・中華民国（台湾）・香港などで発売された家庭用ゲーム機。略記・略称はSFC、スーファミなど[注 1]。日本発売は1990年（平成2年）11月21日、生産終了は2003年（平成15年）9月30日。\n\nファミリーコンピュータの後継機として開発された。同世代機の中では後発であったが、ファミリーコンピュータに引き続き、最多出荷台数を記録した。\n\n北米・欧州・オーストラリア・ブラジルなどでは“Super Nintendo Entertainment System”（スーパーニンテンドーエンターテインメントシステム、略称：Super NES、またはSNES）の名称で発売された。 ";

    std::vector<Instrument> instruments;

    #define t allTextTest

    std::u32string currentTestText = To_UTF32(t);

    for (int i = 0; i < currentTestText.length() && instruments.size() < 256; i+= 12){
        instruments.push_back(Instrument());
        instruments[instruments.size()-1].setName(currentTestText.substr(i, std::min(static_cast<int>(currentTestText.size()-i), 12)));
    }

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

                //TileMatrix textMatrix = font.renderToTiles(currentTestText, std::ceil((event.size.width/scale)/8));

                int width = std::ceil((event.size.width/scale)/8);

                TileMatrix textMatrix = TileMatrix(width, 8, 0x20);

                for (int i = 0; i < width; i+=16){
                    for (int j = 0; j < 8; j++){
                        std::string output;
                        if ((i>>1)+j < instruments.size()){
                            char numchar[5];
                            std::snprintf(numchar, 5, "%02X:", (i>>1)+j);
                            std::string num(numchar);
                            output = num + instruments[(i>>1)+j].getName() + " ";
                        } else { 
                            char numchar[17];
                            std::snprintf(numchar, 17, "%02X:             ", (i>>1)+j);
                            std::string num(numchar);
                            output = num;                        
                        }
                        TileMatrix string = font.renderToTiles(output, 15);
                        textMatrix.copyRect(i, j, std::min(15, width-i), 1, &string, 0, 0);
                    }
                }

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
