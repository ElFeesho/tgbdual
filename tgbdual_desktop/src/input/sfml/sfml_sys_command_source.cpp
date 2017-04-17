//
// Created by Christopher Sawczuk on 17/04/2017.
//

#include <SFML/Window/Event.hpp>
#include "sfml_sys_command_source.h"

sfml_sys_command_source::sfml_sys_command_source(sf::RenderWindow &window) : _window{window} {

}

void sfml_sys_command_source::provideSysCommand(std::function<void()> saveState, std::function<void()> loadState, std::function<void()> toggleSpeed, std::function<void()> quit,
                                                std::function<void()> activateScript, std::function<void()> openConsole) {
    sf::Event e;
    while (_window.pollEvent(e)) {
        if (e.type == sf::Event::Closed) {
            quit();
        }
        else if (e.type == sf::Event::KeyPressed) {
            if (e.key.code == sf::Keyboard::Key::F5) {
                saveState();
            } else if (e.key.code == sf::Keyboard::Key::F5) {
                saveState();
            } else if (e.key.code == sf::Keyboard::Key::F7) {
                loadState();
            } else if (e.key.code == sf::Keyboard::Key::Escape) {
                quit();
            } else if (e.key.code == sf::Keyboard::Key::Tab) {
                toggleSpeed();
            } else if (e.key.code == sf::Keyboard::Key::Space) {
                activateScript();
            } else if (e.key.code == sf::Keyboard::Key::Tilde) {
                openConsole();
            }
        }
    }
}
