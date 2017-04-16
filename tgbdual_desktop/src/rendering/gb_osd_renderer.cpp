//
// Created by Christopher Sawczuk on 16/04/2017.
//

#include <emulator_time.h>
#include <iostream>
#include "gb_osd_renderer.h"

gb_osd_renderer::gb_osd_renderer(tgb::video_renderer *renderer) : _renderer{renderer} {
}

void gb_osd_renderer::render() {
    for (auto &op : pending_operations) {
        op();
    }

    pending_operations.clear();

    renderOSDMessages();
}

void gb_osd_renderer::display_message(const std::string &msg, uint64_t duration) {
    std::cout << msg << std::endl;
    osd_messages.emplace_back(pending_osd_message{emulator_time::current_time() + duration, msg});
}

void gb_osd_renderer::add_rect(const osd_rect &rect) {
    pending_operations.push_back([=] {
        _renderer->fillRect(rect.x(), rect.y(), rect.w(), rect.h(), rect.stroke(), rect.fill());
    });
}

void gb_osd_renderer::add_image(const osd_image &image) {
    pending_operations.push_back([=] {
        _renderer->image(image.name().c_str(), image.x(), image.y());
    });
}

void gb_osd_renderer::add_text(const std::string &text, int16_t x, int16_t y) {
    pending_operations.push_back([=] {
        _renderer->text(text.c_str(), x, y, 0xffffffff);
    });
}

void gb_osd_renderer::renderOSDMessages() {
    int16_t msg_number = 0;
    for (pending_osd_message &osd_msg : osd_messages) {
        int16_t y = (int16_t)(11 + msg_number * 20);
        int16_t x = 11;
        _renderer->text(osd_msg.second.c_str(), x, y, 0xffffffff);

        msg_number++;
    }

    osd_messages.erase(
            remove_if(osd_messages.begin(), osd_messages.end(), [](pending_osd_message &msg) { return emulator_time::current_time() > msg.first; }), osd_messages.end());

}
