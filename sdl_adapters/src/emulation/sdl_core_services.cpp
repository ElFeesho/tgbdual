#include <emulator_time.h>
#include <SDL_ttf.h>
#include "sdl_core_services.h"

sdl_core_services::sdl_core_services() : _videoRenderer{SDL_SetVideoMode(520, 488, 0, SDL_SWSURFACE)}, _gamepadSource{}, _consoleDriver{}, _audioRenderer{}  {

}

tgb::gamepad_source *sdl_core_services::gamepadSource() {
    return &_gamepadSource;
}

tgb::console_driver *sdl_core_services::consoleDriver() {
    return &_consoleDriver;
}

tgb::video_renderer *sdl_core_services::videoRenderer() {
    return &_videoRenderer;
}

tgb::sys_command_source *sdl_core_services::sysCommandSource() {
    return &_gamepadSource;
}

tgb::audio_renderer *sdl_core_services::audioRenderer() {
    return &_audioRenderer;
}

std::unique_ptr<core_services, void(*)(core_services*)> createCoreServices() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    emulator_time::set_time_provider(&SDL_GetTicks);
    emulator_time::set_sleep_provider(&SDL_Delay);
    return std::unique_ptr<core_services, void(*)(core_services*)>(new sdl_core_services(), [](core_services* services){
        delete services;
        TTF_Quit();
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_Quit();
    });
}