#pragma once

#include "input_source.h"

#include <map>

class keyboard_input_source : public input_source
{
public:
	keyboard_input_source();
	uint8_t provide_input(uint8_t initial_state, const SDL_Event &ev);
private:
    std::map<int, int> keyValues;  
};
