#ifndef EASY_PLOT_DRAW_HPP_INCLUDED
#define EASY_PLOT_DRAW_HPP_INCLUDED

#include "easy_plot_common.hpp"
#include "easy_plot_line_spec.hpp"
#include "easy_plot_window_spec.hpp"
#include "../utility/easy_plot_hsv_to_rgb.hpp"
#include "../utility/easy_plot_screenshot.hpp"
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <mutex>
#include <vector>

namespace easy_plot {
    namespace tools {

    #if(0)
        class DrawingData {
        public:
            static inline std::vector<std::shared_ptr<Drawing>> drawings;  /**< Список окон для отрисовки граиков */

            /** \brief Обработчик перерисовки экрана
             */
            static void update_draw() {
                int win_id = glutGetWindow();
                for(size_t i = 0; i < DrawingData::drawings.size(); ++i) {
                    if(DrawingData::drawings[i]->win_id == win_id) {
                        DrawingData::drawings[i]->draw();
                        break;
                    }
                }
            }

            /** \brief Событие закрытия окна
             */
            static void event_closing() {
                int win_id = glutGetWindow();
                for(size_t i = 0; i < DrawingData::drawings.size(); ++i) {
                    if(DrawingData::drawings[i]->win_id == win_id) {
                        DrawingData::drawings[i]->close();
                        DrawingData::drawings.erase(DrawingData::drawings.begin() + i);
                        break;
                    }
                }
            }

            /** \brief Событие движения мыши внутри окна
             */
            static void event_mouse_move(int x, int y) {
                int win_id = glutGetWindow();
                for(size_t i = 0; i < drawings.size(); ++i) {
                    if(DrawingData::drawings[i]->win_id == win_id) {
                        DrawingData::drawings[i]->set_mouse_position(x, y);
                        glutPostRedisplay();
                        break;
                    }
                }
            }

            /** \brief Событие входа или выхода мыши из текущего окна
             */
            static void event_entry(int state) {
                int win_id = glutGetWindow();
                if(state == GLUT_ENTERED) windiw_entry_id = win_id;
                else windiw_entry_id = -1;
                for(size_t i = 0; i < DrawingData::drawings.size(); ++i) {
                    if(DrawingData::drawings[i]->win_id == win_id) {
                        DrawingData::drawings[i]->is_use_mouse = state == GLUT_ENTERED ? true : false;
                        glutPostRedisplay();
                        break;
                    }
                }
            }

            /** \brief Обработчик изменения размера окна
             */
            static void event_reshape(int width, int height) {
                int win_id = glutGetWindow();
                for(size_t i = 0; i < DrawingData::drawings.size(); ++i) {
                    if(DrawingData::drawings[i]->win_id == win_id) {
                        DrawingData::drawings[i]->width = width;
                        DrawingData::drawings[i]->height = height;
                        glViewport(0, 0, width, height);
                        break;
                    }
                }
            }
        };
        #endif

        class Drawing;

        /** \brief Класс для отрисовки графиков
         */
        class Drawing {
        public:
            static inline std::recursive_mutex drawings_mutex;
            static inline std::vector<std::shared_ptr<Drawing>> drawings;   /**< Список окон для отрисовки граиков */
        private:
            static inline int window_screen_x = 0;          /**< Положение окон по умолчанию */
            static inline int window_screen_y = 0;          /**< Положение окон по умолчанию */
            static inline int windiw_entry_id = 0;          /**< ID окна, в котором находится пользователь */
            static inline utility::Screenshot screenshot;
            //static inline std::vector<std::shared_ptr<Drawing>> drawings;   /**< Список окон для отрисовки граиков */

            WindowSpec window_style;                        /**< Стиль окна */
            std::vector<std::vector<double>> raw_data_y;    /**< Данные для рисования всех графиков */
            std::vector<std::vector<double>> raw_data_x;    /**< Данные для рисования всех графиков */
            std::vector<LineSpec> line_style;               /**< Стили линий для всех графиков */

            // для отображения изображений
            std::unique_ptr<float[]> raw_image_data;        /**< Сырые данные изображения */
            std::unique_ptr<float[]> scale_image_data;      /**< Масштабированные данные изображени */
            size_t image_width = 0;                         /**< Ширина изображения */
            size_t image_height = 0;                        /**< Высота изображения */
            //bool is_draw_image = false;                     /**< Флаг отрисовки изображения */


            enum TypesImages {
                CURVE = 0,
                THERMAL_MAP = 1,
            };
            int image_type = CURVE;

            // Минимумы и максимумы данных для рисования графиков
            double min_x = 0.0, max_x = 0.0, min_y = 0.0, max_y = 0.0;

            bool is_window_init = false;                    /**< Флаг инициализации окна */
            bool is_raw_data = false;                       /**< Флаг инициализации данных графика */

            /** \brief Получить нормализованное значение Y
             * \param ny не нормализованное значение
             * \return Нормализованное значение Y
             */
            inline double get_y(double ny) {
                if(max_y == min_y) return 0.0;
                return ((ny - min_y) / (max_y - min_y)) * 2.0 - 1.0;
            }

            /** \brief Получить нормализованное значение X
             * \param nx не нормализованное значение
             * \return Нормализованное значение X
             */
            inline double get_x(double nx) {
                if(max_x == min_x) return 0.0;
                return ((nx - min_x) / (max_x - min_x)) * 2.0 - 1.0;
            }

            static std::string format_axis_value(double v) {
                char buf[48];
                if (std::fabs(v - std::round(v)) < 1e-6)
                    std::snprintf(buf, sizeof(buf), "%.0f", v);
                else
                    std::snprintf(buf, sizeof(buf), "%.4g", v);
                return std::string(buf);
            }

            static std::vector<double> compute_axis_ticks(double vmin, double vmax, int target_count) {
                std::vector<double> ticks;
                if (vmax < vmin)
                    std::swap(vmin, vmax);
                if (std::fabs(vmax - vmin) < 1e-12) {
                    ticks.push_back(vmin);
                    return ticks;
                }
                const int slots = std::max(2, target_count);
                const double raw_step = (vmax - vmin) / (double)(slots - 1);
                const double mag = std::pow(10.0, std::floor(std::log10(raw_step)));
                const double residual = raw_step / mag;
                double nice = 10.0;
                if (residual <= 1.0) nice = 1.0;
                else if (residual <= 2.0) nice = 2.0;
                else if (residual <= 5.0) nice = 5.0;
                const double step = nice * mag;
                double t = std::ceil(vmin / step - 1e-12) * step;
                for (int guard = 0; guard < 64 && t <= vmax + step * 1e-6; ++guard, t += step)
                    ticks.push_back(t);
                if (ticks.empty()) {
                    ticks.push_back(vmin);
                    ticks.push_back(vmax);
                }
                return ticks;
            }

            bool should_draw_tick_label(double pos, double last_pos, double min_gap) const {
                return last_pos < -1.5 || std::fabs(pos - last_pos) >= min_gap;
            }

            double text_width_norm(const std::string& text) const {
                if (width <= 0 || text.empty()) return 0.0;
                return convert_pixel_to_relative_len(
                    glutBitmapLength(window_style.font, (const unsigned char*)text.c_str()), width);
            }

            double text_height_norm() const {
                if (height <= 0) return 0.05;
                return convert_pixel_to_relative_len(glutBitmapHeight(window_style.font), height);
            }

            void draw_chart_decorations() {
                if (image_type != CURVE || !is_raw_data) return;

                const double th = text_height_norm();
                const double gap = th * 0.35;
                glColor3f(window_style.tr, window_style.tg, window_style.tb);

                if (window_style.show_axis_ticks) {
                    const std::vector<double> x_ticks = compute_axis_ticks(
                        min_x, max_x, window_style.axis_tick_count);
                    const std::vector<double> y_ticks = compute_axis_ticks(
                        min_y, max_y, window_style.axis_tick_count);
                    const double tick_len = std::max(0.012, th * 0.35);
                    const double x_label_y = -1.0 - th - gap;
                    const double y_label_x_base = -1.0 - gap * 2.0;
                    const double min_x_gap = th * 1.2;
                    const double min_y_gap = th * 1.15;

                    glBegin(GL_LINES);
                    glColor3f(window_style.fr, window_style.fg, window_style.fb);
                    for (double tx : x_ticks) {
                        const double px = get_x(tx);
                        glVertex2f(px, -1.0);
                        glVertex2f(px, -1.0 - tick_len);
                    }
                    for (double ty : y_ticks) {
                        const double py = get_y(ty);
                        glVertex2f(-1.0, py);
                        glVertex2f(-1.0 - tick_len, py);
                    }
                    glEnd();

                    glColor3f(window_style.tr, window_style.tg, window_style.tb);
                    double last_x_center = -2.0;
                    for (double tx : x_ticks) {
                        const std::string label = format_axis_value(tx);
                        const double px = get_x(tx);
                        const double lw = text_width_norm(label);
                        if (!should_draw_tick_label(px, last_x_center, min_x_gap))
                            continue;
                        render_spaced_bitmap_string(
                            px - lw * 0.5, x_label_y, 0.0,
                            window_style.font, label);
                        last_x_center = px;
                    }

                    double last_y_center = -2.0;
                    for (double ty : y_ticks) {
                        const std::string label = format_axis_value(ty);
                        const double py = get_y(ty);
                        const double lw = text_width_norm(label);
                        if (!should_draw_tick_label(py, last_y_center, min_y_gap))
                            continue;
                        render_spaced_bitmap_string(
                            y_label_x_base - lw, py - th * 0.4, 0.0,
                            window_style.font, label);
                        last_y_center = py;
                    }
                }

                if (!window_style.x_label.empty()) {
                    const double lw = text_width_norm(window_style.x_label);
                    render_spaced_bitmap_string(
                        -lw * 0.5, -1.0 - th * 2.2 - gap, 0.0,
                        window_style.font, window_style.x_label);
                }

                if (!window_style.y_label.empty()) {
                    render_spaced_bitmap_string(
                        -0.98, 0.92 - th, 0.0,
                        window_style.font, window_style.y_label);
                }

                double title_y = 0.96;
                if (!window_style.chart_title.empty()) {
                    const double tw = text_width_norm(window_style.chart_title);
                    render_spaced_bitmap_string(
                        -tw * 0.5, title_y, 0.0,
                        window_style.font, window_style.chart_title);
                    title_y -= th + gap;
                }
                if (!window_style.chart_subtitle.empty()) {
                    const double sw = text_width_norm(window_style.chart_subtitle);
                    const double sx = sw > 1.85 ? -0.92 : -sw * 0.5;
                    render_spaced_bitmap_string(
                        sx, title_y, 0.0,
                        window_style.font, window_style.chart_subtitle);
                    title_y -= th + gap;
                }

                if (window_style.show_legend && !line_style.empty()) {
                    double lx = 0.52;
                    double ly = 0.88;
                    const double seg = 0.09;
                    for (size_t nl = 0; nl < line_style.size(); ++nl) {
                        std::string lab = line_style[nl].legend;
                        if (lab.empty())
                            lab = "series " + std::to_string(nl + 1);

                        glBegin(GL_LINES);
                        glColor3f(line_style[nl].r, line_style[nl].g, line_style[nl].b);
                        glVertex2f(lx, ly);
                        glVertex2f(lx + seg, ly);
                        glEnd();

                        glColor3f(window_style.tr, window_style.tg, window_style.tb);
                        render_spaced_bitmap_string(lx + seg + gap, ly - th * 0.45, 0.0,
                                                    window_style.font, lab);
                        ly -= th * 1.55;
                    }
                }
            }

            /** \brief Получить значения оси X
             * \param size размер массива
             * \return данные оси X
             */
            std::vector<std::vector<int>> get_x_axis_values_for_size(const size_t size) {
                std::vector<int> x(size);
                std::iota(std::begin(x), std::end(x), 0);
                std::vector<std::vector<int>> temp;
                temp.push_back(x);
                return temp;
            }

            /** \brief Получить значения оси X
             * \param size размер массива
             * \return данные оси X
             */
            template<typename T1>
            std::vector<std::vector<int>> get_x_axis_values(const std::vector<std::vector<T1>> & data) {
                std::vector<std::vector<int>> temp(data.size());
                for(size_t  i = 0; i < data.size(); ++i) {
                    temp[i].resize(data[i].size());
                    std::iota(std::begin(temp[i]), std::end(temp[i]), 0);

                }
                return temp;
            }

            template<typename T1>
            std::vector<std::vector<T1>> get_axis_values(const std::vector<T1> &data) {
                std::vector<std::vector<T1>> temp;
                temp.push_back(data);
                return temp;
            }

            template<typename T1>
            std::vector<T1> get_style_values(const T1 &data) {
                std::vector<T1> temp;
                temp.push_back(data);
                return temp;
            }

            /** \brief Напечатать текст
             * \param x позиция в окне по оси X
             * \param y позиция в окне по оси Y
             * \param spacing дополнителный пробел
             * \param font шрифт
             * \param str строка
             */
            void render_spaced_bitmap_string(
                                        float x,
                                        float y,
                                        float spacing,
                                        void *font,
                                        const std::string &str) {
                glRasterPos2f(x, y);
                for(size_t i = 0; i < str.size(); ++i) {
                    glutBitmapCharacter(font, (int)str[i]);
                    x = x + glutBitmapWidth(font, (int)str[i]) + spacing;
                }
            }

            /** \brief Преобразовать пиксели в относительное расстояние
             * Данная функция нужна для отрисовки текста возле курсора мыши
             * \param value Значение в пикселях
             * \param size Максимальное количество пикселей
             * \return относительное расстояние
             */
            double convert_pixel_to_relative_len(int value, size_t size) const {
                const double max_len = 2.0 + window_style.indent * 2.0;
                return ((double)value / (double) size) * max_len;
            }

            /** \brief Преобразовать пиксели в относительное расстояние
             * Данная функция нужна для получения положения курсора мыши
             * \param value Значение в пикселях
             * \param size Максимальное количество пикселей
             * \return относительное расстояние
             */
            double convert_pixel_to_relative(int value, size_t size) {
                const double max_len = 2.0 + window_style.indent * 2.0;
                return ((double)value / (double) size) * max_len - max_len / 2.0;
            }

            public:
            static Drawing* current_instance;       /**< Указатель на класс */
            std::string window_name;                /**< Имя окна графика */
            std::string save_image_name;            /**< Имя файла для сохранения изображения */

            int win_id = -1;    /**< Униклаьный ID графика */
            int width = 0;      /** Актуальная ширина окна */
            int height = 0;     /** Актуальная высота окна */

            double mouse_x = 0.0;   /** Позиция мыши по оси X */
            double mouse_y = 0.0;   /** Позиция мыши по оси Y */
            bool is_use_mouse = false;  /**< Флаг использования мыши в окне */
            bool is_save_image = false; /**< Флаг сохранения изображения */
            bool pending_close = false; /**< Закрыть и удалить в потоке GLUT (не в callback) */

            /** \brief Установить позицию мыши
             * \param x координаты по оси X
             * \param y координаты по оси Y
             */
            void set_mouse_position(int x, int y) {
                if(width == 0 || height == 0) return;
                mouse_x = convert_pixel_to_relative(x, width);
                mouse_y = convert_pixel_to_relative(height - y, height);
            }

            /** \brief Закрыть окно
             * Данная функция нужна для внутреннего использования
             */
            void close() {
                is_window_init = false;
                is_raw_data = false;
                is_use_mouse = false;
            }

            static std::shared_ptr<Drawing> find_by_glut_window(int win_id) {
                for (size_t i = 0; i < drawings.size(); ++i) {
                    if (drawings[i]->win_id == win_id && drawings[i]->is_window_init
                        && !drawings[i]->pending_close)
                        return drawings[i];
                }
                return nullptr;
            }

            static void purge_pending_closed_windows() {
                std::vector<int> win_ids;
                for (size_t i = 0; i < drawings.size();) {
                    if (!drawings[i]->pending_close) {
                        ++i;
                        continue;
                    }
                    if (drawings[i]->win_id >= 0)
                        win_ids.push_back(drawings[i]->win_id);
                    drawings[i]->close();
                    drawings.erase(drawings.begin() + (ptrdiff_t)i);
                }
                for (int wid : win_ids) {
                    glutSetWindow(wid);
                    glutDestroyWindow(wid);
                }
            }

            Drawing() {};

            /** \brief Инициализация графика
             * \param name имя окна
             * \param wstyle парамтеры стиля окна
             * \param x значения графика X
             * \param y значения графика Y
             * \param styles стили линий
             */
            template <typename T1, typename T2>
            void init(const std::string &name, const WindowSpec &wstyle, const std::vector<std::vector<T1>> &x, const std::vector<std::vector<T2>> &y, const std::vector<LineSpec> &styles) {
                is_raw_data = false;
                image_type = CURVE;
                if(x.size() != y.size() || styles.size() != y.size() || name == "" || x[0].size() == 0 || y[0].size() == 0) return;
                max_y = y[0][0];
                min_y = y[0][0];
                max_x = x[0][0];
                min_x = x[0][0];

                raw_data_x.resize(x.size());
                raw_data_y.resize(y.size());
                for(size_t i = 0; i < x.size(); ++i) {
                    if(x[i].size() != y[i].size()) return;
                    raw_data_x[i].resize(x[i].size());
                    raw_data_y[i].resize(y[i].size());
                    for(size_t j = 0; j < x[i].size(); ++j) {
                        raw_data_x[i][j] = x[i][j];
                        raw_data_y[i][j] = y[i][j];
                        max_y = std::max((double)y[i][j], max_y);
                        min_y = std::min((double)y[i][j], min_y);
                        max_x = std::max((double)x[i][j], max_x);
                        min_x = std::min((double)x[i][j], min_x);
                    } // for j
                } // for i

                window_name = name;
                window_style = wstyle;
                line_style = styles;
                if(wstyle.is_zero_x_line && min_y >= 0.0) min_y = 0.0;

                // изменяем min и max по оси Y чтобы был отступ
                double additive = window_style.indent_frame * (max_y - min_y);
                max_y += additive;
                min_y -= additive;

                is_raw_data = true;
                if(is_window_init) glutPostWindowRedisplay(win_id);
                //if(win_id == windiw_entry_id) glutPostRedisplay();
            }

            /** \brief Инициализация графика
             * \param name имя окна
             * \param wstyle стиль окна
             * \param y вектор по оси Y
             * \param style стиль линии
             */
            template <typename T1>
            void init(const std::string &name, const WindowSpec &wstyle, const std::vector<T1> &y, const LineSpec &style) {
                std::vector<LineSpec> vstyle = get_style_values(style);
                std::vector<std::vector<int>> vx = get_x_axis_values_for_size(y.size());
                std::vector<std::vector<T1>> vy = get_axis_values(y);
                init(name, wstyle, vx, vy, vstyle);
            }

            template <typename T1>
            Drawing(const std::string &name, const WindowSpec &wstyle, const std::vector<T1> &y, const LineSpec &style) {
                init(name, wstyle, y, style);
            };

            /** \brief Инициализация графика
             * \param name имя окна
             * \param wstyle стиль окна
             * \param x вектор по оси X
             * \param y вектор по оси Y
             * \param style стиль линии
             */
            template <typename T1, typename T2>
            void init(const std::string &name, const WindowSpec &wstyle, const std::vector<T1> &x, const std::vector<T2> &y, const LineSpec &style) {
                std::vector<LineSpec> vstyle = get_style_values(style);
                std::vector<std::vector<T1>> vx = get_axis_values(x);
                std::vector<std::vector<T2>> vy = get_axis_values(y);
                init(name, wstyle, vx, vy, vstyle);
            }

            template <typename T1, typename T2>
            Drawing(const std::string &name, const WindowSpec &wstyle, const std::vector<T1> &x, const std::vector<T2> &y, const LineSpec &style) {
                init(name, wstyle, x, y, style);
            };

            /** \brief Инициализация графика
             * \param name имя окна
             * \param y
             * \param style стиль линии
             */
            template <typename T1>
            void init(const std::string &name, const WindowSpec &wstyle, const std::vector<std::vector<T1>> &vec, const std::vector<LineSpec> &styles) {
                std::vector<std::vector<int>> vx = get_x_axis_values(vec);
                init(name, wstyle, vx, vec, styles);
            }

            template <typename T1>
            Drawing(const std::string &name, const WindowSpec &wstyle, const std::vector<std::vector<T1>> &vec, const std::vector<LineSpec> &styles) {
                init(name, wstyle, vec, styles);
            };

            /** \brief Инициализируем тепловую карту
             * \param name          Имя окна
             * \param wstyle        Стиль окна
             * \param image_data    Указатель на массив данных
             * \param width         Щирина изображения
             * \param height        Высота изображения
             */
            void init(const std::string &name, const WindowSpec &wstyle, const float *image_data, const size_t width, const size_t height) {
                is_raw_data = false;
                //image_type = CURVE;
                if(name == "" || width == 0 || height == 0) return;
                size_t old_size = image_width * image_height;
                size_t new_size = width * height;
                if(new_size > old_size) {
                    raw_image_data = std::unique_ptr<float[]>(new float[new_size]);
                    scale_image_data = std::unique_ptr<float[]>(new float[new_size]);
                }
                image_width = width;
                image_height = height;
                float *point_raw_image_data = raw_image_data.get();
                std::copy(image_data, image_data + new_size, point_raw_image_data);
                float min_data = std::numeric_limits<float>::max();
                float max_data = std::numeric_limits<float>::lowest();

                for(size_t i = 0; i < new_size; ++i) {
                    if(max_data < point_raw_image_data[i]) max_data = point_raw_image_data[i];
                    if(min_data > point_raw_image_data[i]) min_data = point_raw_image_data[i];
                } // for i

                float scale_factor = max_data == min_data ? 0.0 : 1.0 / (max_data - min_data);
                float *point_scale_image_data = scale_image_data.get();
                for(size_t i = 0; i < new_size; ++i) {
                    point_scale_image_data[i] = (point_raw_image_data[i] - min_data) * scale_factor;
                } // for i

                window_name = name;
                window_style = wstyle;
                image_type = THERMAL_MAP;
                is_raw_data = true;
                if(is_window_init) glutPostWindowRedisplay(win_id);
                //if(win_id == windiw_entry_id) glutPostRedisplay();
            }

            /** \brief Инициализируем изображение
             * \param name имя окна
             * \param wstyle стиль окна
             * \param image_data указатель на массив данных
             * \param width ширина изображения
             * \param height высота изображения
             */
            Drawing(const std::string &name, const WindowSpec &wstyle, const float *image_data, const size_t width, const size_t height) {
                init(name, wstyle, image_data, width, height);
            };

            /** \brief Функция рисования графика
             */
            void draw() {
                if(!is_raw_data) return; // если график не был инициализирован
                glClear(GL_COLOR_BUFFER_BIT);

                // рисуем сетку
                if(image_type == CURVE) {
                    glBegin(GL_LINES);
                    if(window_style.is_grid && window_style.grid_period != 0.0) {
                        glColor3f(window_style.gr, window_style.gg, window_style.gb);
                        for(double x = -1.0; x < 1.0; x += window_style.grid_period) {
                            glVertex2f(x, -1);
                            glVertex2f(x, 1);
                        }
                        for(double y = -1.0; y < 1.0; y += window_style.grid_period) {
                            glVertex2f(-1, y);
                            glVertex2f(1, y);
                        }
                    }
                    glEnd();
                }

                // рисуем линию нуля по осям
                if(image_type == CURVE) {
                    glBegin(GL_LINES);
                    // рисуем линию нуля по оси X
                    if(window_style.is_zero_x_line) {
                        glColor3f(window_style.fr, window_style.fg, window_style.fb);
                        glVertex2f(-1, get_y(0));
                        glVertex2f(1, get_y(0));
                    }
                    // рисуем линию нуля по оси Y
                    if(window_style.is_zero_y_line) {
                        glColor3f(window_style.fr, window_style.fg, window_style.fb);
                        glVertex2f(get_x(0), -1);
                        glVertex2f(get_x(0), 1);
                    }
                    glEnd();
                }

                // для поиска минимальной дистнации до курсора мыши
                double min_distance = 4.0 + 4.0 * window_style.indent;

                double real_mouse_x = 0.0, real_mouse_y = 0.0;
                double mouse_data_x = 0.0;      // значение элемента массива по оси X
                double mouse_data_y = 0.0;      // значение элемента массива по оси Y
                double mouse_image_data = 0.0;  // значение пикселя изображения
                size_t mouse_image_x = 0;
                size_t mouse_image_y = 0;
                int indx_raw_data = 0;

                if(image_type == CURVE) {
                    glBegin(GL_LINES);
                    // рисуем график
                    for(size_t nl = 0; nl < line_style.size(); ++nl) {
                        glLineWidth(line_style[nl].width > 0.0f ? line_style[nl].width : 1.0f);
                        glColor3f(line_style[nl].r, line_style[nl].g, line_style[nl].b);
                        if (raw_data_y[nl].size() < 2)
                            continue;
                        for(size_t i = 0; i + 1 < raw_data_y[nl].size(); ++i) {
                            double x1 = get_x(raw_data_x[nl][i]);
                            double x2 = get_x(raw_data_x[nl][i + 1]);
                            double y1 = get_y(raw_data_y[nl][i]);
                            double y2 = get_y(raw_data_y[nl][i + 1]);
                            if(is_use_mouse) {
                                double distance_xy1 = std::abs(x1 - mouse_x) + std::abs(y1 - mouse_y);
                                double distance_xy2 = std::abs(x2 - mouse_x) + std::abs(y2 - mouse_y);
                                if(distance_xy1 < min_distance) {
                                    mouse_data_x = raw_data_x[nl][i];
                                    mouse_data_y = raw_data_y[nl][i];
                                    indx_raw_data = nl;
                                    real_mouse_x = x1;
                                    real_mouse_y = y1;
                                    min_distance = distance_xy1;
                                }
                                if(distance_xy2 < min_distance) {
                                    mouse_data_x = raw_data_x[nl][i + 1];
                                    mouse_data_y = raw_data_y[nl][i + 1];
                                    indx_raw_data = nl;
                                    real_mouse_x = x2;
                                    real_mouse_y = y2;
                                    min_distance = distance_xy2;
                                }
                            }
                            glVertex2f(x1, y1);
                            glVertex2f(x2, y2);
                        }
                    }
                    glEnd();
                    glLineWidth(1.0f);
                } else {
                    // рисуем изображение
                    glBegin(GL_QUADS);
                    glColor3f(window_style.ir, window_style.ig, window_style.ib);
                    double step_x = 2.0/(double)image_width;
                    double step_y = 2.0/(double)image_height;
                    size_t image_ind = 0;
                    for(size_t x = 0; x < image_width; ++x) {
                        double x1 = -1.0 + (double)x * step_x;
                        double x2 = x1 + step_x;
                        for(size_t y = 0; y < image_height; ++y, ++image_ind) {
                            double y2 = 1.0 - (double)y * step_y;
                            double y1 = y2 - step_y;

                            if(!window_style.is_color_heatmap) {
                                glColor3f(
                                    window_style.ir * scale_image_data[image_ind],
                                    window_style.ig * scale_image_data[image_ind],
                                    window_style.ib * scale_image_data[image_ind]);
                            } else {
                                float r, g, b;
                                if(window_style.is_invert_color_heatmap) utility::hsv_to_rgb(1.0 - scale_image_data[image_ind], r, g, b);
                                else utility::hsv_to_rgb(scale_image_data[image_ind], r, g, b);
                                glColor3f(r,g,b);
                            }
                            glVertex2f(x1, y2);
                            glVertex2f(x1, y1);
                            glVertex2f(x2, y1);
                            glVertex2f(x2, y2);
                        }
                    }
                    glEnd();

                    if(is_use_mouse) {
                        mouse_image_x = (mouse_x + 1.0)/step_x;
                        mouse_image_y = (1.0 - mouse_y)/step_y;
                        double x1 = -1.0 + (double)mouse_image_x * step_x;
                        double x2 = x1 + step_x;
                        double y2 = 1.0 - (double)mouse_image_y * step_y;
                        double y1 = y2 - step_y;
                        real_mouse_x = (x1 + x2) / 2.0;
                        real_mouse_y = (y1 + y2) / 2.0;
                        mouse_image_data = raw_image_data[mouse_image_x * image_height + mouse_image_y];
                    }
                }

                // рисуем сетку для изображения
                if(image_type == THERMAL_MAP && window_style.is_grid) {
                    glBegin(GL_LINES);
                    glColor3f(window_style.gr, window_style.gg, window_style.gb);
                    double step_x = 2.0/(double)image_width;
                    for(double x = -1.0; x < 1.0; x += step_x) {
                        glVertex2f(x, -1);
                        glVertex2f(x, 1);
                    }
                    double step_y = 2.0/(double)image_height;
                    for(double y = -1.0; y < 1.0; y += step_y) {
                        glVertex2f(-1, y);
                        glVertex2f(1, y);
                    }
                    glEnd();
                }

                // рисуем мышь
                if(is_use_mouse) {
                    glBegin(GL_LINES);
                    glColor3f(window_style.mr, window_style.mg, window_style.mb);
                    glVertex2f(-1, real_mouse_y);
                    glVertex2f(1, real_mouse_y);
                    glVertex2f(real_mouse_x, -1);
                    glVertex2f(real_mouse_x, 1);
                    glEnd();
                }

                // рисуем рамку
                glBegin(GL_LINES);
                glColor3f(window_style.fr, window_style.fg, window_style.fb);
                glVertex2f(-1, 1);
                glVertex2f(1, 1);

                glVertex2f(1, 1);
                glVertex2f(1, -1);

                glVertex2f(1, -1);
                glVertex2f(-1, -1);

                glVertex2f(-1, -1);
                glVertex2f(-1, 1);
                glEnd();

                draw_chart_decorations();

                // если естьмышь, отобразим линию
                if(is_use_mouse) {
                    if(image_type == CURVE) {
                        std::string text_line("line ");
                        text_line += std::to_string(indx_raw_data);
                        std::string text_x(std::to_string(mouse_data_x));
                        std::string text_y(std::to_string(mouse_data_y));

                        glColor3f(window_style.tr, window_style.tg, window_style.tb);

                        const double dh = convert_pixel_to_relative_len(glutBitmapHeight(window_style.font), height);
                        const double OFFSET_Y = 0.01;

                        const double dw1 = convert_pixel_to_relative_len(glutBitmapLength(window_style.font, (const unsigned char*)text_line.c_str()), width);
                        const double dw2 = convert_pixel_to_relative_len(glutBitmapLength(window_style.font, (const unsigned char*)text_x.c_str()), width);
                        const double dw3 = convert_pixel_to_relative_len(glutBitmapLength(window_style.font, (const unsigned char*)text_y.c_str()), width);
                        const double dw = std::max(dw1, std::max(dw2, dw3));
                        const double OFFSET_X = 0.02;

                        // реализуем правильное размещение текста на экране в зависимости от расположения курсора
                        double string_x = real_mouse_x;
                        double string_y = real_mouse_y;
                        if((1.0 - real_mouse_x - OFFSET_X) < dw) {
                            string_x = real_mouse_x - dw - OFFSET_X;
                        }
                        if((1.0 - real_mouse_y - OFFSET_Y) < (dh * 3)) {
                            string_y = real_mouse_y - dh * 3 - OFFSET_Y;
                        }

                        render_spaced_bitmap_string(string_x, string_y + dh * 2 + OFFSET_Y, 0.0, window_style.font, text_line);
                        render_spaced_bitmap_string(string_x, string_y + dh + OFFSET_Y, 0.0, window_style.font, text_x);
                        render_spaced_bitmap_string(string_x, string_y + OFFSET_Y, 0.0, window_style.font, text_y);
                    } else {
                        std::string text_line("pixel ");
                        text_line += std::to_string(mouse_image_x);
                        text_line += " ";
                        text_line += std::to_string(mouse_image_y);
                        std::string text_pixel(std::to_string(mouse_image_data));

                        glColor3f(window_style.tr, window_style.tg, window_style.tb);
                        const double dh = convert_pixel_to_relative_len(glutBitmapHeight(window_style.font), height);
                        const double OFFSET_Y = 0.02;

                        const double dw = convert_pixel_to_relative_len(glutBitmapLength(window_style.font, (const unsigned char*)text_line.c_str()), width);
                        const double OFFSET_X = 0.02;

                        // реализуем правильное размещение текста на экране в зависимости от расположения курсора
                        double string_x = real_mouse_x;
                        double string_y = real_mouse_y;
                        if((1.0 - real_mouse_x - OFFSET_X) < dw) {
                            string_x = real_mouse_x - dw - OFFSET_X;
                        }
                        if((1.0 - real_mouse_y - OFFSET_Y) < dh * 2) {
                            string_y = real_mouse_y - dh * 2 - OFFSET_Y;
                        }

                        render_spaced_bitmap_string(string_x, string_y + dh * 1 + OFFSET_Y, 0.0, window_style.font, text_line);
                        render_spaced_bitmap_string(string_x, string_y + dh * 0 + OFFSET_Y, 0.0, window_style.font, text_pixel);
                    }
                }

                /*
                glFlush();

                if(is_save_image) {
                    screenshot.print_screen(save_image_name, width, height, utility::Screenshot::TypesFormats::USE_PPM);
                    is_save_image = false;
                }
                */
            }

            void flush() {
                glFlush();
            }
#if(1)
            /** \brief Обработчик перерисовки экрана
             */
            static void update_draw() {
                const int win_id = glutGetWindow();
                std::shared_ptr<Drawing> target;
                {
                    std::lock_guard<std::recursive_mutex> lock(drawings_mutex);
                    target = find_by_glut_window(win_id);
                }
                if (!target)
                    return;
                target->draw();
                target->flush();
            }

            /** \brief Событие закрытия окна (только пометка; удаление — в потоке GLUT) */
            static void event_closing() {
                const int win_id = glutGetWindow();
                std::lock_guard<std::recursive_mutex> lock(drawings_mutex);
                for (size_t i = 0; i < drawings.size(); ++i) {
                    if (drawings[i]->win_id == win_id) {
                        drawings[i]->pending_close = true;
                        drawings[i]->close();
                        return;
                    }
                }
            }

            /** \brief Событие движения мыши внутри окна */
            static void event_mouse_move(int x, int y) {
                const int win_id = glutGetWindow();
                bool redraw = false;
                {
                    std::lock_guard<std::recursive_mutex> lock(drawings_mutex);
                    std::shared_ptr<Drawing> target = find_by_glut_window(win_id);
                    if (target) {
                        target->set_mouse_position(x, y);
                        redraw = true;
                    }
                }
                if (redraw)
                    glutPostRedisplay();
            }

            /** \brief Событие входа или выхода мыши из текущего окна */
            static void event_entry(int state) {
                const int win_id = glutGetWindow();
                bool redraw = false;
                {
                    std::lock_guard<std::recursive_mutex> lock(drawings_mutex);
                    if (state == GLUT_ENTERED)
                        windiw_entry_id = win_id;
                    else
                        windiw_entry_id = -1;
                    std::shared_ptr<Drawing> target = find_by_glut_window(win_id);
                    if (target) {
                        target->is_use_mouse = (state == GLUT_ENTERED);
                        redraw = true;
                    }
                }
                if (redraw)
                    glutPostRedisplay();
            }

            /** \brief Обработчик изменения размера окна */
            static void event_reshape(int width, int height) {
                if (width <= 0 || height <= 0)
                    return;
                const int win_id = glutGetWindow();
                std::lock_guard<std::recursive_mutex> lock(drawings_mutex);
                std::shared_ptr<Drawing> target = find_by_glut_window(win_id);
                if (target) {
                    target->width = width;
                    target->height = height;
                    glViewport(0, 0, width, height);
                }
            }
#endif
            /** \brief Обновить состояние окна
             */
            void update_window() {
                if (pending_close)
                    return;
                if (!is_window_init && is_save_image)
                    is_save_image = false;
                if (is_window_init) {
                    glutSetWindow(win_id);
                    glutMainLoopEvent();
                    return;
                }
                // необходима инициализация окна
                glutInitWindowSize(
                        window_style.width,
                        window_style.height);
                // узнаем размеры экрана
                int screen_w = glutGet(GLUT_SCREEN_WIDTH);
                int screen_h = glutGet(GLUT_SCREEN_HEIGHT);
                const int OFFSET_W = 16;
                const int OFFSET_H = 44;
                // проверяем, вписываемся ли мы в монитор
                if((window_screen_x + (int)window_style.width + OFFSET_W) > screen_w) {
                    window_screen_x = 0;
                    window_screen_y += ((int)window_style.height + OFFSET_H);
                    if((window_screen_y + (int)window_style.height + OFFSET_H) > screen_h) {
                        window_screen_y = 0;
                    }
                }
                glutInitWindowPosition(window_screen_x, window_screen_y);
                window_screen_x += (window_style.width + OFFSET_W);

                win_id = glutCreateWindow(window_name.c_str());
                glutSetWindow(win_id);

                glClearColor(window_style.br, window_style.bg, window_style.bb, 1.0);
                glMatrixMode(GL_PROJECTION); /*Настроим 2-х мерный вид*/
                glLoadIdentity();

                glOrtho(
                        -1.0 - window_style.indent, 1.0  + window_style.indent,
                        -1.0 - window_style.indent, 1.0  + window_style.indent,
                        -1.0, 1.0);
                ::glutDisplayFunc(update_draw);
                ::glutCloseFunc(event_closing);
                ::glutReshapeFunc(event_reshape);
                ::glutPassiveMotionFunc(event_mouse_move);
                ::glutEntryFunc(event_entry);
                glutMainLoopEvent();
                width = (int)window_style.width;
                height = (int)window_style.height;
                glutReshapeWindow((int)window_style.width, (int)window_style.height);
                is_window_init = true; // ставим флаг инициализации окна
            }
        }; // Drawing
    }; // tools
}; // easy_plot

#endif // EASY_PLOT_DRAW_HPP_INCLUDED
