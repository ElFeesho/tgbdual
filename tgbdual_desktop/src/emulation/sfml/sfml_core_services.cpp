//
// Created by Christopher Sawczuk on 17/04/2017.
//

#include <emulator_time.h>
#include "sfml_core_services.h"

sfml_core_services::sfml_core_services() : _window{sf::VideoMode(520, 488), "tgbdual"}, _videoRenderer{_window}, _gamepadSource{_window}, _sysCommandSource{_window}, _consoleDriver{_window} {

}

tgb::gamepad_source *sfml_core_services::gamepadSource() {
    return &_gamepadSource;
}

tgb::console_driver *sfml_core_services::consoleDriver() {
    return &_consoleDriver;
}

tgb::video_renderer *sfml_core_services::videoRenderer() {
    return &_videoRenderer;
}

tgb::sys_command_source *sfml_core_services::sysCommandSource() {
    return &_sysCommandSource;
}

tgb::audio_renderer *sfml_core_services::audioRenderer() {
    return &_audioRenderer;
}

std::unique_ptr<core_services, void(*)(core_services*)> createCoreServices() {
    static sf::Clock _clock;
    emulator_time::set_time_provider([&] {
        return _clock.getElapsedTime().asMilliseconds();
    });

    emulator_time::set_sleep_provider([&](uint32_t sleep) {
       sf::sleep(sf::milliseconds(sleep));
    });

    return std::unique_ptr<core_services, void(*)(core_services*)>(new sfml_core_services, [](core_services* services){
        delete services;
    });
}