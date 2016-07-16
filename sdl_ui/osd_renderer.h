//
// Created by Christopher Sawczuk on 15/07/2016.
//

#pragma once

#include <cstdint>
#include <string>

class osd_renderer
{
public:
	virtual ~osd_renderer() {}
	virtual void display_message(const std::string &msg, uint64_t duration) = 0;
};
