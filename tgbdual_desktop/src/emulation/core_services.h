#pragma once

#include <input/gamepad_source.h>
#include <input/console_driver.h>
#include <rendering/video_renderer.h>
#include <input/sys_command_source.h>
#include <rendering/audio_renderer.h>

#include <memory>

class core_services {
public:
    virtual ~core_services() = default;

    virtual tgb::gamepad_source *gamepadSource() = 0;

    virtual tgb::console_driver *consoleDriver() = 0;

    virtual tgb::video_renderer *videoRenderer() = 0;

    virtual tgb::sys_command_source *sysCommandSource() = 0;

    virtual tgb::audio_renderer *audioRenderer() = 0;
};

std::unique_ptr<core_services, void (*)(core_services *)> createCoreServices();