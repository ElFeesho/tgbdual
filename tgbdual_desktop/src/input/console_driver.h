#pragma once

#include <functional>

namespace tgb {
    class console_driver {
    public:
        enum class CommandKey {
            UP,
            DOWN,
            LEFT,
            RIGHT,
            TAB,
            RETURN,
            CLOSE_CONSOLE,
            BACKSPACE
        };

        using key_down = std::function<void(char)>;
        using key_up = std::function<void(char)>;
        using commandkey_down = std::function<void(CommandKey)>;
        using commandkey_up = std::function<void(CommandKey)>;

        virtual ~console_driver() = default;

        virtual void update(key_down, key_up, commandkey_down, commandkey_up) = 0;
    };
}