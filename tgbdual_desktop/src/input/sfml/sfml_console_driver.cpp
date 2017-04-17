//
// Created by Christopher Sawczuk on 17/04/2017.
//

#include <SFML/Window/Event.hpp>
#include <map>
#include <iostream>
#include "sfml_console_driver.h"

sfml_console_driver::sfml_console_driver(sf::RenderWindow &window) : _window{window} {

}

void sfml_console_driver::update(tgb::console_driver::key_down down, tgb::console_driver::key_up up, tgb::console_driver::commandkey_down commandkey_down,
                                 tgb::console_driver::commandkey_up commandkey_up) {


    std::map<sf::Keyboard::Key, tgb::console_driver::CommandKey> commandKeys = {
            {sf::Keyboard::Key::Up,        tgb::console_driver::CommandKey::UP},
            {sf::Keyboard::Key::Down,      tgb::console_driver::CommandKey::DOWN},
            {sf::Keyboard::Key::Left,      tgb::console_driver::CommandKey::LEFT},
            {sf::Keyboard::Key::Right,     tgb::console_driver::CommandKey::RIGHT},
            {sf::Keyboard::Key::Return,    tgb::console_driver::CommandKey::RETURN},
            {sf::Keyboard::Key::Tab,       tgb::console_driver::CommandKey::TAB},
            {sf::Keyboard::Key::Tilde,     tgb::console_driver::CommandKey::CLOSE_CONSOLE},
            {sf::Keyboard::Key::BackSpace, tgb::console_driver::CommandKey::BACKSPACE}
    };

    sf::Event e;
    while (_window.pollEvent(e)) {
        if (e.type == sf::Event::KeyPressed) {
            auto key = e.key.code;
            if (commandKeys.find(key) != commandKeys.end()) {
                commandkey_down(commandKeys[key]);
            }
        } else if (e.type == sf::Event::KeyReleased) {
            auto key = e.key.code;
            if (commandKeys.find(key) != commandKeys.end()) {
                commandkey_up(commandKeys[key]);
            }
        }
        else if (e.type == sf::Event::TextEntered) {

            sf::Uint32 keyValue = e.text.unicode;
            if (keyValue != '\t' && keyValue != '\b' && keyValue != '\n' && keyValue != '\r') {
                down(keyValue);
                up(keyValue);
            }
        }
    }

}
