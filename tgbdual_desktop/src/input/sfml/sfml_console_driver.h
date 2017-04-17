#pragma once

#include <input/console_driver.h>
#include <SFML/Graphics/RenderWindow.hpp>

class sfml_console_driver : public tgb::console_driver {
public:
    sfml_console_driver(sf::RenderWindow &window);
    void update(key_down down, key_up up, commandkey_down commandkey_down, commandkey_up commandkey_up) override;
private:
    sf::RenderWindow &_window;
};