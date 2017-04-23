#include <SFML/Window/Event.hpp>
#include <map>
#include "sfml_gamepad_source.h"

uint8_t sfml_gamepad_source::provideState() {
    uint8_t state = 0;
    std::map<sf::Keyboard::Key, uint8_t> keyMap {
            { sf::Keyboard::Key::Z, 0x01},
            { sf::Keyboard::Key::X, 0x02},
            { sf::Keyboard::Key::RShift, 0x04},
            { sf::Keyboard::Key::Return, 0x08},
            { sf::Keyboard::Key::Down, 0x10},
            { sf::Keyboard::Key::Up, 0x20},
            { sf::Keyboard::Key::Left, 0x40},
            { sf::Keyboard::Key::Right, 0x80}
    };

    for(auto &pair : keyMap)
    {
        if (sf::Keyboard::isKeyPressed(pair.first))
        {
            state |= pair.second;
        }
    }

    return state;
}
