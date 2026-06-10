#define _GLIBCXX_USE_WCHAR_T 0

#include <vector>
#include <string>
#include <thread>

#include "easy_plot.hpp"



void easy_plot_init(int *argc, char **argv) {
    easy_plot::EasyPlot::init(argc, argv);
}

int easy_plot_xy(const char *name, const double *x, const double *y, int n,
                 const char *x_label, const char *y_label) {
    if (!name || !x || !y || n < 2)
        return 1;

    std::vector<double> xv(x, x + n);
    std::vector<double> yv(y, y + n);

    easy_plot::WindowSpec ws;
    ws.br = ws.bg = ws.bb = 1.0;
    ws.ir = ws.ig = ws.ib = 1.0;
    ws.tr = ws.tg = ws.tb = 0.0;
    ws.gr = ws.gg = ws.gb = 0.75;
    ws.fr = ws.fg = ws.fb = 0.0;
    ws.x_label = x_label ? x_label : "X";
    ws.y_label = y_label ? y_label : "Y";
    ws.show_legend = false;

    easy_plot::LineSpec ls(0.0, 0.0, 1.0);
    return easy_plot::EasyPlot::plot(name, ws, xv, yv, ls);
}

void easy_plot_wait(void) {
    easy_plot::EasyPlot::wait_until_all_windows_closed();
}
