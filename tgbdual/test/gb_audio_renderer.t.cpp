#include <gtest/gtest.h>
#include <rendering/gb_audio_renderer.h>

class fake_sound_provider : public sound_provider {
public:
    void populate_audio_buffer(short *buf, int samples) override {
        memset(buf, 0xffu, samples);
    }
};

class mock_audio_renderer : public tgb::audio_renderer {
public:
    void provideFillBufferCallback(fill_buffer_cb cb) override {
        this->fillCb = cb;
    }

    fill_buffer_cb fillCb;
};

class GbAudioRenderer : public ::testing::Test {
public:
    mock_audio_renderer mockAudioRenderer;
    gb_audio_renderer audioRenderer{&mockAudioRenderer};
    fake_sound_provider fakeSoundProvider;
};

TEST_F(GbAudioRenderer, willUseSoundProviderToPopulateBuffer) {
    audioRenderer.connect_audio_provider(&fakeSoundProvider);

    unsigned short soundBuffer[16]{};
    mockAudioRenderer.fillCb((unsigned char*)soundBuffer, 32);
    for (int i = 0; i < 16; i++) {
        EXPECT_EQ(0xffffu, soundBuffer[i]);
    }
}

TEST_F(GbAudioRenderer, whenNoSoundProviderIsSet_bufferRemainsUntouched) {
    unsigned short soundBuffer[16]{};
    mockAudioRenderer.fillCb((unsigned char*)soundBuffer, 32);
    for (int i = 0; i < 16; i++) {
        EXPECT_EQ(0x0u, soundBuffer[i]);
    }
}
