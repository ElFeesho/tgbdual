//
// Created by Christopher Sawczuk on 16/07/2016.
//

#include "script_context.h"

script_context::script_context(osd_renderer *osd) : _osd{osd} {

}

void script_context::print_string(const std::string &msg) {
	_osd->display_message(msg, 1500);
}
