// library
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <filesystem>
#include <string>
#include <vector>
#include <cmath>

// class : piano
class Piano
{
    // structure : note
    struct Note
    {
        // sound buffer (one buffer for all notes)
        inline static sf::SoundBuffer buffer;

        // sound
        sf::Sound sound;

        // keyboard
        sf::Keyboard::Scan keyboard;

        // constructor
        Note() : sound(buffer) {}
    };

    // notes
    Note notes[25];
    int transpose = 0;

    // shape
    sf::RectangleShape rectangle;

    // paths to sound files
    std::vector<std::filesystem::path> paths;
    int current = 4;

    // font & text
    sf::Font font;
    sf::Text text;

    // function : load current sound file
    void LoadSound()
    {
        if      (current < 0)              current = paths.size() - 1;
        else if (current >= paths.size())  current = 0;

        if (not Note::buffer.loadFromFile(paths[current]))
            throw std::runtime_error("Can't load sound file");
    }

    // function : set pitch of notes
    void SetPitch()
    {
        for (int i = 0;  i < 25;  i++)
            notes[i].sound.setPitch(std::pow(2.f, (transpose + i - 12)/12.f));
    }

    // function : align text
    void AlignText(float x, float y)
    {
        auto bounds = text.getLocalBounds();
        text.setOrigin({
            bounds.position.x + x*bounds.size.x,
            y*text.getCharacterSize()
        });
    }

public:

    // constructor
    Piano() : font("C:/Windows/Fonts/arial.ttf"), text(font)
    {
        // get paths to sound files
        for (auto const& entry : std::filesystem::directory_iterator("Sounds"))
            if (entry.path().extension() == ".mp3")
                paths.push_back(entry.path());

        // load sound file
        LoadSound();

        // set pitch of notes
        SetPitch();

        // set keyboard
        using namespace sf::Keyboard;
        notes[ 0].keyboard = Scan::LShift;     // C
        notes[ 1].keyboard = Scan::Grave;      // C#
        notes[ 2].keyboard = Scan::Tab;        // D
        notes[ 3].keyboard = Scan::Num1;       // D#
        notes[ 4].keyboard = Scan::Q;          // E
        notes[ 5].keyboard = Scan::W;          // F
        notes[ 6].keyboard = Scan::Num3;       // F#
        notes[ 7].keyboard = Scan::E;          // G
        notes[ 8].keyboard = Scan::Num4;       // G#
        notes[ 9].keyboard = Scan::R;          // A
        notes[10].keyboard = Scan::Num5;       // A#
        notes[11].keyboard = Scan::T;          // B
        notes[12].keyboard = Scan::Y;          // C
        notes[13].keyboard = Scan::Num7;       // C#
        notes[14].keyboard = Scan::U;          // D
        notes[15].keyboard = Scan::Num8;       // D#
        notes[16].keyboard = Scan::I;          // E
        notes[17].keyboard = Scan::O;          // F
        notes[18].keyboard = Scan::Num0;       // F#
        notes[19].keyboard = Scan::P;          // G
        notes[20].keyboard = Scan::Hyphen;     // G#
        notes[21].keyboard = Scan::LBracket;   // A
        notes[22].keyboard = Scan::Equal;      // A#
        notes[23].keyboard = Scan::RBracket;   // B
        notes[24].keyboard = Scan::Backslash;  // C
    }

    // function : play sounds with keyboard
    int PlaySounds(sf::Keyboard::Scan keyboard)
    {
        for (int i = 0;  i < 25;  i++)
        {
            if (keyboard == notes[i].keyboard)
            {
                notes[i].sound.play();
                return i;
            }
        }

        // default return
        return -1;
    }

    // function : change sound with keyboard
    void ChangeSound(sf::Keyboard::Scan keyboard)
    {
        using namespace sf::Keyboard;

        // sound file
        if      (keyboard == Scan::Up  ) { current--; LoadSound(); }
        else if (keyboard == Scan::Down) { current++; LoadSound(); }

        // transpose
        else if (keyboard == Scan::Left ) { transpose--; SetPitch(); }
        else if (keyboard == Scan::Right) { transpose++; SetPitch(); }
    }

    // function : draw on window
    void Draw(sf::RenderWindow& window)
    {
        // size
        float w = window.getSize().x;
        float h = window.getSize().y / 2.f;

        // position
        float x = 0.f;
        float y = window.getSize().y - h;

        // bar
        float bar_w = w;
        float bar_h = h * 0.2f;
        rectangle.setSize({bar_w, bar_h});
        rectangle.setPosition({x, y});
        rectangle.setOutlineThickness(0.f);
        rectangle.setFillColor({32, 32, 32});
        window.draw(rectangle);

        // file name
        text.setString(paths[current].stem().string());
        text.setPosition({x + 20.f, y + bar_h/2});
        text.setCharacterSize(bar_h/2);
        AlignText(0.f, 0.6f);
        window.draw(text);

        // transpose
        text.setString(std::to_string(transpose));
        text.setPosition({w - 20.f, y + bar_h/2});
        AlignText(1.f, 0.6f);
        window.draw(text);

        // keys
        y += bar_h;
        float key_w = w / 15.f;
        float key_h = h - bar_h;
        rectangle.setOutlineThickness(-1.f);
        rectangle.setOutlineColor(sf::Color::Black);
        for (int i = 0;  i < 25;  i++)
        {
            // white keys
            int j = i % 12;
            if (j != 1 and j != 3 and j != 6 and j != 8 and j != 10)
            {
                rectangle.setSize({key_w, key_h});
                rectangle.setPosition({x, y});
                if (sf::Keyboard::isKeyPressed(notes[i].keyboard))
                    rectangle.setFillColor({0, 170, 255});
                else
                    rectangle.setFillColor(sf::Color::White);
                window.draw(rectangle);

                // black keys (on the left of white keys)
                if (j != 0 and j != 5)
                {
                    rectangle.setSize({key_w/2, key_h*0.6f});
                    rectangle.setPosition({x - key_w/4, y});
                    if (sf::Keyboard::isKeyPressed(notes[i-1].keyboard))
                        rectangle.setFillColor({0, 170, 255});
                    else
                        rectangle.setFillColor(sf::Color::Black);
                    window.draw(rectangle);
                }

                // next position
                x += key_w;
            }
        }
    }
};

// class : shooter
class Shooter
{
    // structure : shot
    struct Shot
    {
        // state
        bool active = false;

        // position
        float x;

        // index
        int index;
    };

    // maximum shots
    static constexpr int n = 40;

    // current shot
    int current = 0;

    // shots
    Shot shots[n];

    // velocity
    float velocity = 200.f;

    // shape
    sf::CircleShape    circle;
    sf::RectangleShape rectangle;
    
    // map index to vertical position
    int map[25];

public:

    // constructor
    Shooter()
    {
        // create map
        int count = -1;
        for (int i = 0;  i < 25;  i++)
        {
            // count natural notes
            int j = i % 12;
            if (j != 1 and j != 3 and j != 6 and j != 8 and j != 10)
                count++;

            // store count
            map[i] = count;
        }
    }

    // function : shoot
    void Shoot(int index)
    {
        // invalid index
        if (index < 0 or index >= 25)
            return;

        // set active & initial position
        shots[current].active = true;
        shots[current].x      = 0.f;
        shots[current].index  = index;

        // next shot
        current++;
        if (current >= n)
            current = 0;
    }

    // function : update actives shots
    void Update(float dt, float range = 2000.f)
    {
        for (int i = 0;  i < n;  i++)
        {
            // skip inactive shots
            if (not shots[i].active)
                continue;

            // move at constant velocity
            shots[i].x += velocity * dt;

            // inactive when out of range
            if (shots[i].x > range)
                shots[i].active = false;
        }
    }

    // function : draw active shots on window
    void Draw(sf::RenderWindow& window)
    {
        // size
        float w = window.getSize().x;
        float h = window.getSize().y / 2.f;

        // shape
        float r = h / 30;
        circle.setRadius(r);
        circle.setOrigin({0.f, r});
        
        // lines
        rectangle.setSize({w, 1.f});
        rectangle.setOrigin({0.f, 0.5f});
        rectangle.setFillColor({128, 128, 128});
        for (int i = 1;  i < 6;  i++)
        {
            rectangle.setPosition({0.f, h/2 - 2*r*i});
            window.draw(rectangle);
            rectangle.setPosition({0.f, h/2 + 2*r*i});
            window.draw(rectangle);
        }

        // shots
        for (int i = 0;  i < n;  i++)
        {
            // skip inactive shots
            if (not shots[i].active)
                continue;

            float x = w - shots[i].x;
            float y = h/2 - r*(map[shots[i].index] - 7);
            int   j = shots[i].index % 12;
            if (j == 1 or j == 3 or j == 6 or j == 8 or j == 10)
                circle.setFillColor({0, 170, 255});
            else
                circle.setFillColor(sf::Color::White);
            circle.setPosition({x, y});
            window.draw(circle);
        }
    }
};

// main program
int main()
{
    // create piano
    Piano piano;

    // create shooter
    Shooter shooter;
    
    // create window
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Title");

    // disable key repeat
    window.setKeyRepeatEnabled(false);

    // start clock
    sf::Clock clock;

    // window loop
    while (window.isOpen())
    {
        // handle events
        while (auto event = window.pollEvent())
        {
            // on close button press
            if (event->is<sf::Event::Closed>())
            {
                // close window
                window.close();
            }

            // on window resize
            else if (auto resized = event->getIf<sf::Event::Resized>())
            {
                // update view
                window.setView(sf::View(sf::FloatRect({0.f, 0.f}, sf::Vector2f(resized->size))));
            }

            // on keyboard press
            else if (auto key = event->getIf<sf::Event::KeyPressed>())
            {
                // play sound & shoot
                int index = piano.PlaySounds(key->scancode);
                shooter.Shoot(index);

                // change sound
                piano.ChangeSound(key->scancode);
            }
        }

        // delta time
        float dt = clock.getElapsedTime().asSeconds();
        if (dt >= 1.f / 60.f)
        {
            clock.restart();    

            // update
            shooter.Update(dt);

            // fill window with color
            window.clear({64, 64, 64});

            // draw
            piano  .Draw(window);
            shooter.Draw(window);

            // update display
            window.display();
        }
    }

    // program end successfully
    return 0;
}