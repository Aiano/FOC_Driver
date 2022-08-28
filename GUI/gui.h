#ifndef FOC_DRIVER_GUI_H
#define FOC_DRIVER_GUI_H
#include "main.h"
#include "FOC.h"

void gui_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void gui_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t fill, uint16_t color);
void gui_draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void gui_draw_init();
void gui_draw_mode_selection(FOC_CONTROL_MODE mode);
void
gui_draw_button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char *string, uint16_t color, uint16_t bgcolor);
void gui_draw_parameter(uint16_t x, uint16_t y, const char* item, float value);
void gui_draw_test();

#endif //FOC_DRIVER_GUI_H
