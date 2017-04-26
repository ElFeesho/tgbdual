#pragma once

#include <functional>

namespace tgb {
    class audio_renderer {
    public:
        using fill_buffer_cb = std::function<void(uint8_t *, size_t)>;
        virtual ~audio_renderer()= default;

        virtual void provideFillBufferCallback(fill_buffer_cb) = 0;
    };
}

