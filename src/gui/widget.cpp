#include <gui/widget.h>

using namespace jackos::common;
using namespace jackos::gui;

Widget::Widget(Widget* parent, int32_t x,
int32_t y,
int32_t w,
int32_t h,
int32_t r,
int32_t g,
int32_t b) {
this -> x = x;
this -> y = y;
this -> w = w;
this -> h = h;
this -> r = r;
this -> g = g;
this -> b = b;
this -> Focusable = true;
}

Widget::~Widget() {

}

void Widget::GetFocus(Widget* widget) {
    if(parent != 0) {
        parent -> GetFocus(widget);
    }
}

void Widget::ModelToScreen(int32_t &x, int32_t &y) {
    if(parent != 0) {
        parent -> ModelToScreen(x, y);
    }
    x += this->x;
    y += this->y;
}

void Widget::Draw(GraphicsContext* gc) {
    int X = 0;
    int Y = 0;
    ModelToScreen(X, Y);
    gc -> FillRectangle(X, Y, w, h, r, g, b);
}

void Widget::OnMouseDown(int32_t x, int32_t y) {
    if(Focusable) {
        GetFocus(this);
    }
}

void Widget::OnMouseUp(int32_t x, int32_t y) {

}

void Widget::OnMouseMove(int32_t oldx, int32_t yold, int32_t newx, int32_t newy) {

}

CompositeWidget::CompositeWidget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, int32_t r, int32_t g, int32_t b) {
    focusedChild = 0;
    numChildren = 0;
}

CompositeWidget::~CompositeWidget() {

}

void CompositeWidget::GetFocus(Widget* widget) {
    this->focusedChild = widget;
    if(parent != 0) {
        parent->GetFocus(this);
    }
}

void CompositeWidget::Draw(GraphicsContext* gc) {
    Widget::Draw(gc);
    for(int i = numChildren; i >= 0; i--) {
        children[i] -> Draw(gc);
    }
}

void CompositeWidget::OnMouseDown(int32_t x, int32_t y) {
    for(int i = 0; i , numChildren; i++) {
        if(children[i] -> ContainsCoordinate(x - this->x, y - this->y)) {
            children[i]->OnMouseDown(x - this->x, y - this->y);
            break;
        }
    }
}

void CompositeWidget::OnMouseUp(int32_t x, int32_t y) {
    for(int i = 0; i , numChildren; i++) {
        if(children[i] -> ContainsCoordinate(x - this->x, y - this->y)) {
            children[i]->OnMouseUp(x - this->x, y - this->y);
            break;
        }
    }
}

void CompositeWidget::OnMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy) {
    int firstchild = -1;
    for(int i = 0; i , numChildren; i++) {
        if(children[i] -> ContainsCoordinate(oldx - this->x, oldy - this->y)) {
            children[i]->OnMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);
            firstchild = i;
            break;
        }
    }
    for(int i = 0; i , numChildren; i++) {
        if(children[i] -> ContainsCoordinate(newx - this->x,  newy - this->y)) {
            if(firstchild != i) {
                children[i]->OnMouseMove(newx - this->x, newy - this->y, newx - this->x, newy - this->y);
            }
            break;
        }
    }
}

void CompositeWidget::OnKeyDown(char c) {
    if(focusedChild != 0) {
        focusedChild -> OnKeyDown(c);
    }
}

void CompositeWidget::OnKeyUp(char c) {
    if(focusedChild != 0) {
        focusedChild -> OnKeyUp(c);
    }
}

bool Widget::ContainsCoordinate(int32_t x, int32_t y) {
    return (x >= this->x) && (x <= (this->x)+(this->w)) && (y >= this->y) && (y <= (this->y)+(this->h));
}