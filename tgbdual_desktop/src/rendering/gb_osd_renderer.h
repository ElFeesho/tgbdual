#pragma once

#include <osd_renderer.h>
#include <vector>
#include <functional>

#include "video_renderer.h"

class gb_osd_renderer : public osd_renderer{
public:
    gb_osd_renderer(tgb::video_renderer *renderer);

    void render();

    void display_message(const std::string &msg, uint64_t duration) override;

    void add_rect(const osd_rect &rect) override;

    void add_image(const osd_image &image) override;

    void add_text(const std::string &text, int16_t x, int16_t y) override;
private:
    using draw_op = std::function<void()>;
    using pending_osd_message = std::pair<uint64_t, std::string>;
    std::vector<pending_osd_message> osd_messages;
    std::vector<draw_op> pending_operations;
    tgb::video_renderer *_renderer;

    void renderOSDMessages();
};
