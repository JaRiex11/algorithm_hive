#ifndef EASY_PLOT_LINE_SPEC_HPP_INCLUDED
#define EASY_PLOT_LINE_SPEC_HPP_INCLUDED

#include <string>

namespace easy_plot {

    /** \brief Класс для хранения данных стиля линии
     */
    class LineSpec {
        public:
        //@{
        /** цвет линии (R G B) */
        double r = 0.0, g = 1.0, b = 1.0, a = 1.0;
        //@}

        std::string legend; /**< Подпись серии в легенде */

        /** \brief Инициализация параметров линии по умолчанию
         * Данный конструктор инициализирует линию цветом по умолчанию
         */
        LineSpec() {};

        /** \brief Инициализация параметров линии
         * \param red уровеньк расного
         * \param green уровень зеленого
         * \param blue уровень голубого
         * \param alpha прозрачность
         */
        LineSpec(double red, double green, double blue, double alpha = 1.0) {
            r = red;
            g = green;
            b = blue;
            a = alpha;
        }

        LineSpec(double red, double green, double blue, const std::string& legend_label, double alpha = 1.0) {
            r = red;
            g = green;
            b = blue;
            a = alpha;
            legend = legend_label;
        }
    };

}; // easy_plo

#endif // EASY_PLOT_LINE_SPEC_HPP_INCLUDED
