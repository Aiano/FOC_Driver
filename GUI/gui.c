#include <string.h>
#include <stdlib.h>
#include "gui.h"
#include "image.h"
#include "FOC_utils.h"


const char foc_mode_name[FOC_CONTROL_MODE_NUM][20] = {
        "OPEN POS",
        "OPEN VEL",
        "TORQUE",
        "VELOCITY",
        "POSITION",
        "SPRING",
        "SPRING DMP",
        "DAMP",
        "KNOB",
        "0 RESIST"
};


void gui_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    if (x1 == x2) { // slope == infinite
        if (y2 < y1) { // Make sure that y1 <= y2
            uint8_t temp = y2;
            y2 = y1;
            y1 = temp;
        }
        for (uint8_t y0 = y1; y0 <= y2; y0++) {
            ST7735_DrawPixel(x1, y0, color);
        }
        return;
    }

    if (x2 < x1) { // Make sure that x1 <= x2
        // Swap two variable in-place
        uint8_t temp = x2;
        x2 = x1;
        x1 = temp;

        temp = y2;
        y2 = y1;
        y1 = temp;
    }


    for (uint8_t x0 = x1; x0 <= x2; x0++) { // iterate each pixel of the line
        uint8_t y0 = (y2 - y1) * (x0 - x1) / (x2 - x1) + y1;
        ST7735_DrawPixel(x0, y0, color);
    }
    return;
}

void gui_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t fill, uint16_t color) {
    // Clock wise from left top
    if (fill) {
        for (int i = y1; i <= y2; i++) {
            for (int j = x1; j <= x2; j++) {
                ST7735_DrawPixel(j, i, color);
            }
        }
        return;
    }

    gui_draw_line(x1, y1, x2, y1, color); // Line1
    gui_draw_line(x2, y1, x2, y2, color); // Line2
    gui_draw_line(x2, y2, x1, y2, color); // Line3
    gui_draw_line(x1, y2, x1, y1, color); // Line4
    return;
}

void gui_draw_triangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color) {
    // Clock wise from left top

    gui_draw_line(x1, y1, x2, y2, color); // Line1
    gui_draw_line(x2, y2, x3, y3, color); // Line2
    gui_draw_line(x3, y3, x1, y1, color); // Line3
}

void gui_draw_init(const char *init_info, uint8_t refresh) {
    static const int delay_time = 200;

    if (refresh) {
        ST7735_FillScreen(BACKGROUND_COLOR);
        ST7735_DrawImage(0, 0, ST7735_HEIGHT, ST7735_HEIGHT, (uint16_t *) head_portrait_80x80);

        ST7735_WriteString(80, 20, "FOC", Font_11x18, TITLE_COLOR, BACKGROUND_COLOR);
        ST7735_WriteString(80, 38, "Driver", Font_11x18, TITLE_COLOR, BACKGROUND_COLOR);
        gui_draw_line(80, 18, 150, 18, TEXT_COLOR);
        gui_draw_line(80, 68, 150, 68, TEXT_COLOR);

        ST7735_WriteString(80, 56, init_info, Font_7x10, TEXT_COLOR, BACKGROUND_COLOR);
    } else {
        ST7735_FillRectangle(80, 56, 80, 10, BACKGROUND_COLOR);
        ST7735_WriteString(80, 56, init_info, Font_7x10, TEXT_COLOR, BACKGROUND_COLOR);
    }
}

void gui_draw_mode_selection(FOC_CONTROL_MODE mode) {

    static const int y = 65, width = 23, height = 12;
    ST7735_FillScreenFast(BACKGROUND_COLOR);

    gui_draw_line(0, y - 3, 180, y - 3, TEXT_COLOR);
    gui_draw_line(0, 20, 180, 20, TEXT_COLOR);

    gui_draw_button(10, y, width, height, "lef", TEXT_COLOR, BACKGROUND_COLOR);
    gui_draw_button(50, y, width, height, "cfm", TEXT_COLOR, BACKGROUND_COLOR);
//    gui_draw_button(90, y, width, height, "cel", ST7735_BLACK, ST7735_WHITE);
    gui_draw_button(130, y, width, height, "rig", TEXT_COLOR, BACKGROUND_COLOR);

    ST7735_WriteString(0, 0, "Mode Selection", Font_11x18, TEXT_COLOR, BACKGROUND_COLOR);


    ST7735_WriteString(0, 30, foc_mode_name[mode], Font_16x26, TITLE_COLOR, BACKGROUND_COLOR);

    ST7735_FillRectangle(0, 57, ST7735_WIDTH * (mode + 1) / FOC_CONTROL_MODE_NUM, 3, BAR_COLOR);

//    gui_draw_parameter(0, 57, "A", 10);
}

void gui_draw_position_mode(float angle, uint8_t refresh) {
    static const int y = 65, width = 23, height = 12;
    if (refresh) {
        ST7735_FillScreenFast(BACKGROUND_COLOR);

        gui_draw_line(0, y - 3, 180, y - 3, TEXT_COLOR);
        gui_draw_line(0, 20, 180, 20, TEXT_COLOR);

        gui_draw_button(10, y, width, height, "lef", TEXT_COLOR, BACKGROUND_COLOR);
//    gui_draw_button(50, y, width, height, "cfm", ST7735_BLACK, ST7735_WHITE);
        gui_draw_button(90, y, width, height, "cel", TEXT_COLOR, BACKGROUND_COLOR);
        gui_draw_button(130, y, width, height, "rig", TEXT_COLOR, BACKGROUND_COLOR);

        ST7735_WriteString(0, 0, "Position Mode", Font_11x18, TITLE_COLOR, BACKGROUND_COLOR);
    } else {
        ST7735_FillRectangle(0, 21, 180, 40, BACKGROUND_COLOR);
    }
    // show parameters
    gui_draw_parameter(0, 22, "Angle", angle * _RADIAN_TO_DEGREE, 0, 0);
}

void
gui_draw_button(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const char *string, uint16_t color, uint16_t bgcolor) {

    gui_draw_line(x, y, x + w, y, color);
    gui_draw_line(x + w, y, x + w, y + h, color);
    gui_draw_line(x + w, y + h, x, y + h, color);
    gui_draw_line(x, y + h, x, y, color);
    ST7735_WriteString(x + 1, y + 1, string, Font_7x10, color, bgcolor);
}

void gui_draw_parameter(uint16_t x, uint16_t y, const char *item, int16_t value, uint8_t change, uint8_t selected) {
    char data[20], num[10];
    strcpy(data, item);
    strcat(data, ":");
    itoa(value, num, 10);
    strcat(data, num);
    if (selected) {
        if (change) {
            ST7735_WriteString(x, y, data, Font_7x10, TEXT_COLOR, SELECTED_COLOR);
        } else {
            ST7735_WriteString(x, y, data, Font_7x10, BACKGROUND_COLOR, SELECTED_COLOR);
        }
    } else {
        ST7735_WriteString(x, y, data, Font_7x10, TEXT_COLOR, BACKGROUND_COLOR);
    }
}

void gui_draw_knob_mode(uint8_t sector_num, uint8_t k, uint8_t max_force, uint8_t select_param, uint8_t change,
                        uint8_t refresh) {
    static const int y = 65, width = 23, height = 12;
    static const char param_name[3][10] = {
            "Sector",
            "k",
            "max_F",
    };
    static uint8_t values[3];

    if (refresh) {
        ST7735_FillScreenFast(BACKGROUND_COLOR);

        gui_draw_line(0, y - 3, 180, y - 3, TEXT_COLOR);
        gui_draw_line(0, 20, 180, 20, TEXT_COLOR);

        gui_draw_button(10, y, width, height, "lef", TEXT_COLOR, BACKGROUND_COLOR);
//    gui_draw_button(50, y, width, height, "cfm", ST7735_BLACK, ST7735_WHITE);
        gui_draw_button(90, y, width, height, "cel", TEXT_COLOR, BACKGROUND_COLOR);
        gui_draw_button(130, y, width, height, "rig", TEXT_COLOR, BACKGROUND_COLOR);

        ST7735_WriteString(0, 0, "Knob Mode", Font_11x18, TITLE_COLOR, BACKGROUND_COLOR);
    } else {
        ST7735_FillRectangle(0, 21, 180, 40, BACKGROUND_COLOR);
    }

    // show parameters
    values[0] = sector_num, values[1] = k, values[2] = max_force;
    for (int i = 0; i < 3; ++i) {
        if (select_param == i + 1) {
            gui_draw_parameter(0, 22 + i * 10, param_name[i], values[i], change, 1);
        } else {
            gui_draw_parameter(0, 22 + i * 10, param_name[i], values[i], change, 0);
        }
    }
}




