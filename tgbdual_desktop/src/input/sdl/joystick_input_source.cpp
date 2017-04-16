#include "joystick_input_source.h"

static const int16_t DEADZONE = 16384;

joystick_input_source::joystick_input_source()
{
	joyMap[1] = 0x01;
	joyMap[2] = 0x02;
	joyMap[8] = 0x04;
	joyMap[9] = 0x08;
}

uint8_t joystick_input_source::provide_input(uint8_t initial_state, const SDL_Event &ev)
{
	if (ev.type == SDL_JOYBUTTONDOWN || ev.type == SDL_JOYBUTTONUP)
	{
		auto joyevent = ev.jbutton;
		if (joyMap.find(joyevent.button) != joyMap.end())
		{
			if (ev.type == SDL_JOYBUTTONDOWN)
			{
				initial_state |= joyMap[joyevent.button];
			}
			else
			{
				initial_state &= ~joyMap[joyevent.button];
			}
		}
	}
	else
	{
		auto joyaxis = ev.jaxis;
        if (joyaxis.axis == 0)
        {
            if(joyaxis.value > DEADZONE)
            {
                initial_state |= 0x80;
            }
            else
            {
                initial_state &= ~0x80;
            }

            if(joyaxis.value < -DEADZONE)
            {
                initial_state |= 0x40;
            }
            else
            {
                initial_state &= ~0x40;
            }
        }
        if (joyaxis.axis == 1)
        {
            if(joyaxis.value > DEADZONE)
            {
                initial_state |= 0x10;
            }
            else
            {
                initial_state &= ~0x10;
            }

            if(joyaxis.value < -DEADZONE)
            {
                initial_state |= 0x20;
            }
            else
            {
                initial_state &= ~0x20;
            }
        }
	}
	return initial_state;
}


