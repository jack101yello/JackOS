#ifndef __jackos_gui_window_H
#define __jackos_gui_window_H

#include <gui/widget.h>
#include <drivers/mouse.h>

namespace jackos {
    namespace gui {
        class Window : public CompositeWidget {
            protected:
                bool Dragging;
            public:
                Window(Widget* parent,
                jackos::common::int32_t x,
                jackos::common::int32_t y,
                jackos::common::int32_t w,
                jackos::common::int32_t h,
                jackos::common::uint8_t r,
                jackos::common::uint8_t g,
                jackos::common::uint8_t b);
                ~Window();
                void OnMouseDown(jackos::common::int32_t x, jackos::common::int32_t y, jackos::common::uint8_t button);
                void OnMouseUp(jackos::common::int32_t x, jackos::common::int32_t y, jackos::common::uint8_t button);
                void OnMouseMove(jackos::common::int32_t oldx, jackos::common::int32_t oldy, jackos::common::int32_t newx, jackos::common::int32_t newy);
        };
    }
}

#endif