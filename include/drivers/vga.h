#ifndef __jackos_drivers_vga_H
#define __jackos_drivers_vga_H

#include <common/types.h>
#include <hardware/port.h>
#include <drivers/driver.h>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 200
#define COLOR_DEPTH 8

namespace jackos {
    namespace drivers {
        class VideoGraphicsArray {
            protected:
                jackos::hardware::Port8Bit miscport;
                jackos::hardware::Port8Bit crtcIndexPort;
                jackos::hardware::Port8Bit crtcDataPort;
                jackos::hardware::Port8Bit sequencerIndexPort;
                jackos::hardware::Port8Bit sequencerDataPort;
                jackos::hardware::Port8Bit graphicsControllerIndexPort;
                jackos::hardware::Port8Bit graphicsControllerDataPort;
                jackos::hardware::Port8Bit attributeControllerIndexPort;
                jackos::hardware::Port8Bit attributeControllerReadPort;
                jackos::hardware::Port8Bit attributeControllerWritePort;
                jackos::hardware::Port8Bit attributeControllerResetPort;
                jackos::common::uint32_t framebuffer[SCREEN_WIDTH][SCREEN_HEIGHT];

                void WriteRegisters(jackos::common::uint8_t* registers);
                jackos::common::uint8_t* GetFrameBufferSegment();
                virtual jackos::common::uint8_t GetColorIndex(jackos::common::uint8_t r, jackos::common::uint8_t g, jackos::common::uint8_t b);
            
            public:
                VideoGraphicsArray();
                ~VideoGraphicsArray();

                virtual int GetMode(jackos::common::uint32_t width, jackos::common::uint32_t height, jackos::common::uint32_t colordepth);
                virtual bool SetMode(jackos::common::uint32_t width, jackos::common::uint32_t height, jackos::common::uint32_t colordepth);
                virtual void PutPixel(jackos::common::int32_t x, jackos::common::int32_t y, jackos::common::uint8_t r, jackos::common::uint8_t g, jackos::common::uint8_t b);
                virtual void PutPixel(jackos::common::int32_t x, jackos::common::int32_t y, jackos::common::uint8_t colorcode);
                virtual void FillRectangle(jackos::common::uint32_t x, jackos::common::uint32_t y, jackos::common::uint32_t w, jackos::common::uint32_t h, jackos::common::uint8_t r, jackos::common::uint8_t g, jackos::common::uint8_t b);
                virtual void DrawFrame(jackos::common::uint32_t width, jackos::common::uint32_t height);
                virtual void DrawCharacter(const char character, jackos::common::uint32_t x, jackos::common::uint32_t y);
                virtual void Print(const char* message, jackos::common::uint32_t x, jackos::common::uint32_t y);
        };
    }
}

#endif