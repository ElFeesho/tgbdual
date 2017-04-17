#pragma once

#include <SFML/Graphics.hpp>

#include <emulation/core_services.h>
#include <rendering/sfml_video_renderer.h>
#include <input/sfml_gamepad_source.h>
#include <input/sfml_sys_command_source.h>
#include <input/sfml_console_driver.h>
#include <rendering/sfml_audio_renderer.h>
#include <rendering/dummy_audio_renderer.h>

class sfml_core_services : public core_services {
public:
    sfml_core_services();

    tgb::gamepad_source *gamepadSource() override;

    tgb::console_driver *consoleDriver() override;

    tgb::video_renderer *videoRenderer() override;

    tgb::sys_command_source *sysCommandSource() override;

    tgb::audio_renderer *audioRenderer() override;
private:
    sf::Font _font;
    sf::RenderWindow _window;
    sfml_gamepad_source _gamepadSource;
    sfml_video_renderer _videoRenderer;
    sfml_console_driver _consoleDriver;
    sfml_sys_command_source _sysCommandSource;
    dummy_audio_renderer _audioRenderer;

};
