#pragma once

#include "x16G.cpp"

namespace x16G {

    void OPT3 _idraw_aa(uint32_t x, uint32_t y, uint32_t color) {
        int px = map[x];
        int py = map[y];

        float fpx = mapf[x];
        float fpy = mapf[y];

        float a = Math::pow(px-fpx, 2);
        float b = Math::pow(py-fpy, 2);

        float dist = Math::sqrt(a+b);

        Screen::draw(px, py, apply_alpha_coeff(color, Math::sqrt2-dist));
    }

    static void OPT3 plot_point(int px, int py, float brightness, uint32_t color) {
        if (brightness > 1.0f) brightness = 1.0f;
        if (brightness < 0.0f) brightness = 0.0f;

        uint32_t final_color = apply_alpha_coeff(color, brightness);
        _idraw_aa(px, py, final_color);
    }

    void OPT3 _idraw_smline_aa(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color) {
        int x0 = x1;
        int y0 = y1;
        int x1_screen = x2;
        int y1_screen = y2;

        int dx = Math::abs(x1_screen - x0);
        int dy = Math::abs(y1_screen - y0);

        int sx = (x0 < x1_screen) ? 1 : -1;
        int sy = (y0 < y1_screen) ? 1 : -1;

        int err = dx - dy;
        int err2;

        int x = x0;
        int y = y0;

        //_idraw_aa(x1_screen--, y1_screen--, color);
        //_idraw_aa(x--, y--, color);

        while (true) {
            _idraw_aa(x, y, color);

            if (x == x1_screen && y == y1_screen) break;

            err2 = 2 * err;
            if (err2 > -dy) {
                err -= dy;
                x += sx;
            }
            if (err2 < dx) {
                err += dx;
                y += sy;
            }
        }
    }

    void OPT3 _idraw_line_aa(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color) {
        int x0 = x1;
        int y0 = y1;
        int x1_screen = x2;
        int y1_screen = y2;

        bool steep = Math::abs(y1_screen - y0) > Math::abs(x1_screen - x0);

        if (steep) {
            int temp;
            temp = x0; x0 = y0; y0 = temp;
            temp = x1_screen; x1_screen = y1_screen; y1_screen = temp;
        }

        if (x0 > x1_screen) {
            int temp;
            temp = x0; x0 = x1_screen; x1_screen = temp;
            temp = y0; y0 = y1_screen; y1_screen = temp;
        }

        int dx = x1_screen - x0;
        int dy = y1_screen - y0;
        float gradient = (dx == 0) ? 1.0f : (float)dy / (float)dx;

        int xend = x0;
        int yend = y0 + gradient * (xend - x0);
        int xgap = 1;

        int xpxl1 = xend;
        int ypxl1 = (int)yend;

        if (steep) {
            plot_point(ypxl1, xpxl1, (1.0f - (yend - ypxl1)) * xgap, color);
            plot_point(ypxl1 + 1, xpxl1, (yend - ypxl1) * xgap, color);
        } else {
            plot_point(xpxl1, ypxl1, (1.0f - (yend - ypxl1)) * xgap, color);
            plot_point(xpxl1, ypxl1 + 1, (yend - ypxl1) * xgap, color);
        }

        xend = x1_screen;
        yend = y1_screen + gradient * (xend - x1_screen);
        xgap = 1;

        int xpxl2 = xend;
        int ypxl2 = (int)yend;

        if (steep) {
            plot_point(ypxl2, xpxl2, (1.0f - (yend - ypxl2)) * xgap, color);
            plot_point(ypxl2 + 1, xpxl2, (yend - ypxl2) * xgap, color);
        } else {
            plot_point(xpxl2, ypxl2, (1.0f - (yend - ypxl2)) * xgap, color);
            plot_point(xpxl2, ypxl2 + 1, (yend - ypxl2) * xgap, color);
        }

        for (int x = xpxl1 + 1; x < xpxl2; x++) {
            float y = y0 + gradient * (x - x0);
            int ypxl = (int)y;
            float brightness1 = 1.0f - (y - ypxl);
            float brightness2 = y - ypxl;

            if (steep) {
                plot_point(ypxl, x, brightness1, color);
                plot_point(ypxl + 1, x, brightness2, color);
            } else {
                plot_point(x, ypxl, brightness1, color);
                plot_point(x, ypxl + 1, brightness2, color);
            }
        }
    }

    void OPT3 _idraw_rect_aa(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t color) {
        _idraw_rect(x, y, w+1, h+1, color);

        _idraw_smline_aa(x, y, x+w, y, color);
        _idraw_smline_aa(x, y, x, y+h, color);
        _idraw_smline_aa(x, y+h, x+w, y+h, color);
        _idraw_smline_aa(x+w, y, x+w, y+h, color);
        volatile uint64_t pix = 1_d;

    }
}