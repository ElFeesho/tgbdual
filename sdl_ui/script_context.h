//
// Created by Christopher Sawczuk on 16/07/2016.
//

#pragma once

#include "osd_renderer.h"

class script_context {
public:
	script_context(osd_renderer *osd);

	void print_string(const std::string&);

private:
	osd_renderer *_osd;
};

