#ifndef __jackos_gui_widget_H
#define __jackos_gui_widget_H

#include <common/types.h>
#include <common/graphicscontext.h>
#include <drivers/keyboard.h>

namespace jackos {
    namespace gui {
        class Widget : public jackos::drivers::KeyboardEventHandler {
            protected:
                Widget* parent;
                jackos::common::int32_t x;
                jackos::common::int32_t y;
                jackos::common::int32_t w;
                jackos::common::int32_t h;

                jackos::common::uint8_t r;
                jackos::common::uint8_t g;
                jackos::common::uint8_t b;
                bool Focusable;
            
            public:
                Widget(Widget* parent, jackos::common::int32_t x,
                jackos::common::int32_t y,
                jackos::common::int32_t w,
                jackos::common::int32_t h,
                jackos::common::uint8_t r,
                jackos::common::uint8_t g,
                jackos::common::uint8_t b);
                ~Widget();
                virtual void GetFocus(Widget* widget);
                virtual void ModelToScreen(jackos::common::int32_t &x, jackos::common::int32_t &y);
                virtual void Draw(jackos::common::GraphicsContext* gc);
                virtual void OnMouseDown(jackos::common::int32_t x, jackos::common::int32_t y, jackos::common::uint8_t button);
                virtual void OnMouseUp(jackos::common::int32_t x, jackos::common::int32_t y, jackos::common::uint8_t button);
                virtual void OnMouseMove(jackos::common::int32_t oldx, jackos::common::int32_t oldy, jackos::common::int32_t newx, jackos::common::int32_t newy);
                virtual bool ContainsCoordinate(jackos::common::int32_t x, jackos::common::int32_t y);
        };

        class CompositeWidget : public Widget {
            private:
                Widget* children[100]; // We don't have a heap yet
                int numChildren;
                Widget* focusedChild;
            public:
                CompositeWidget(Widget* parent,
                    jackos::common::int32_t x,
                    jackos::common::int32_t y,
                    jackos::common::int32_t w,
                    jackos::common::int32_t h,
                    jackos::common::uint8_t r,
                    jackos::common::uint8_t g,
                    jackos::common::uint8_t b);
                ~CompositeWidget();
                virtual void GetFocus(Widget* widget);
                virtual bool AddChild(Widget* child);
                virtual void Draw(jackos::common::GraphicsContext* gc);
                virtual void OnMouseDown(jackos::common::int32_t x, jackos::common::int32_t y, jackos::common::uint8_t button);
                virtual void OnMouseUp(jackos::common::int32_t x, jackos::common::int32_t y, jackos::common::uint8_t button);
                virtual void OnMouseMove(jackos::common::int32_t oldx, jackos::common::int32_t yold, jackos::common::int32_t newx, jackos::common::int32_t newy);
                virtual void OnKeyDown(char c);
                virtual void OnKeyUp(char c);
        };
    }
}

#endif