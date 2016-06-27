#pragma once

class sound_renderer {
   public:
    virtual void render(short *buf, int samples) = 0;
};