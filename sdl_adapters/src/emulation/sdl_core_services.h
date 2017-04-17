#pragma once

#include <emulation/core_services.h>
#include <rendering/sdl_video_renderer.h>
#include <rendering/sdl_audio_renderer.h>
#include <input/sdl_gamepad_source.h>
#include <input/sdl_console_driver.h>

class sdl_core_services : public core_services {
public:
    sdl_core_services();

    tgb::gamepad_source *gamepadSource() override;

    tgb::console_driver *consoleDriver() override;

    tgb::video_renderer *videoRenderer() override;

    tgb::sys_command_source *sysCommandSource() override;

    tgb::audio_renderer *audioRenderer() override;
private:
    sdl_video_renderer _videoRenderer;
    sdl_gamepad_source _gamepadSource;
    sdl_console_driver _consoleDriver;
    sdl_audio_renderer _audioRenderer;
};
