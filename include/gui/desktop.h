#ifndef __jackos_gui_desktop_H
#define __jackos_gui_desktop_H

#include <gui/widget.h>
#include <drivers/mouse.h>
#include <common/types.h>

namespace jackos {
    namespace gui {
        class Desktop : public CompositeWidget, public jackos::drivers::MouseEventHandler {
            protected:
                jackos::common::uint32_t MouseX, MouseY;
            public:
                Desktop(jackos::common::int32_t width, jackos::common::int32_t height, jackos::common::uint8_t r, jackos::common::uint8_t g, jackos::common::uint8_t b);
                ~Desktop();
                void Draw(jackos::common::GraphicsContext* gc);
                void OnMouseDown(jackos::common::uint8_t button);
                void OnMouseUp(jackos::common::uint8_t button);
                void OnMouseMove(jackos::common::int32_t x, jackos::common::int32_t y);
        };
    }
}

#endif