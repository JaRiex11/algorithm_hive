#ifndef EASY_PLOT_C_H
#define EASY_PLOT_C_H

#ifdef __cplusplus
extern "C" {
#endif

void easy_plot_init(int *argc, char **argv);

int easy_plot_xy(const char *name, const double *x, const double *y, int n,
                 const char *x_label, const char *y_label);

void easy_plot_wait(void);

#ifdef __cplusplus
}
#endif

#endif
