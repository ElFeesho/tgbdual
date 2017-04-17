#pragma once

#include <input/sys_command_source.h>
#include <SFML/Graphics/RenderWindow.hpp>

class sfml_sys_command_source : public tgb::sys_command_source {
public:
    sfml_sys_command_source(sf::RenderWindow &window);

    void provideSysCommand(std::function<void()> saveState, std::function<void()> loadState, std::function<void()> toggleSpeed, std::function<void()> quit, std::function<void()> activateScript,
                           std::function<void()> openConsole) override;
private:
    sf::RenderWindow &_window;
};
