#ifndef __jackos_gui_widget_H
#define __jackos_gui_widget_H

#include <common/types.h>
#include <common/graphicscontext.h>

namespace jackos {
    namespace gui {
        class Widget {
            protected:
                Widget* parent;
                jackos::common::int32_t x;
                jackos::common::int32_t y;
                jackos::common::int32_t w;
                jackos::common::int32_t h;

                jackos::common::int32_t r;
                jackos::common::int32_t g;
                jackos::common::int32_t b;
                bool Focusable;
            
            public:
                Widget(Widget* parent, jackos::common::int32_t x,
                jackos::common::int32_t y,
                jackos::common::int32_t w,
                jackos::common::int32_t h,
                jackos::common::int32_t r,
                jackos::common::int32_t g,
                jackos::common::int32_t b);
                ~Widget();
                virtual void GetFocus(Widget* widget);
                virtual void ModelToScreen(jackos::common::int32_t &x, jackos::common::int32_t &y);
                virtual void Draw(GraphicsContext* gc);
                virtual void OnMouseDown(jackos::common::int32_t x, jackos::common::int32_t y);
                virtual void OnMouseUp(jackos::common::int32_t x, jackos::common::int32_t y);
                virtual void OnMouseMove(jackos::common::int32_t oldx, jackos::common::int32_t yold, jackos::common::int32_t newx, jackos::common::int32_t newy);
                virtual void OnKeyDown(char c);
                virtual void OnKeyUp(char c);
                virtual bool ContainsCoordinate(int32_t x, int32_t y);
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
                    jackos::common::int32_t r,
                    jackos::common::int32_t g,
                    jackos::common::int32_t b);
                ~CompositeWidget();
                virtual void GetFocus(Widget* widget);
                virtual void Draw(GraphicsContext* gc);
                virtual void OnMouseDown(jackos::common::int32_t x, jackos::common::int32_t y);
                virtual void OnMouseUp(jackos::common::int32_t x, jackos::common::int32_t y);
                virtual void OnMouseMove(jackos::common::int32_t oldx, jackos::common::int32_t yold, jackos::common::int32_t newx, jackos::common::int32_t newy);
                virtual void OnKeyDown(char c);
                virtual void OnKeyUp(char c);
        };
    }
}

#endif