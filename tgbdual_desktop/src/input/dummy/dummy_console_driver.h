#pragma once

#include <input/console_driver.h>

class dummy_console_driver : public tgb::console_driver {
public:
    void update(key_down down, key_up up, commandkey_down commandkey_down, commandkey_up commandkey_up) override;
};
