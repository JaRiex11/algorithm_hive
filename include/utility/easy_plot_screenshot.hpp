#ifndef EASY_PLOT_SCRENNSHOT_HPP_INCLUDED
#define EASY_PLOT_SCRENNSHOT_HPP_INCLUDED

#include "GL/freeglut.h"
#include <cstddef>
#include <cstdio>
#include <string>
#include <vector>

namespace easy_plot {
namespace utility {

bool write_rgb_png_file(const char* filename, uint32_t width, uint32_t height, const GLubyte* rgb_top_down);

/** \brief Класс для создания скриншотов */
class Screenshot {
private:
    GLubyte* pixels = nullptr;

    static bool ends_with_ci(const std::string& name, const char* suffix) {
        const size_t n = std::string(suffix).size();
        if (name.size() < n)
            return false;
        for (size_t i = 0; i < n; ++i) {
            char a = name[name.size() - n + i];
            char b = suffix[i];
            if (a >= 'A' && a <= 'Z')
                a = char(a - 'A' + 'a');
            if (b >= 'A' && b <= 'Z')
                b = char(b - 'A' + 'a');
            if (a != b)
                return false;
        }
        return true;
    }

    void screenshot_ppm(const char* filename, uint32_t width, uint32_t height, GLubyte** img_pixels) noexcept {
        const size_t format_nchannels = 3;
        FILE* f = std::fopen(filename, "w");
        if (!f)
            return;
        std::fprintf(f, "P3\n%u %u\n%d\n", width, height, 255);
        *img_pixels = (GLubyte*)std::realloc(*img_pixels, format_nchannels * width * height);
        glReadPixels(0, 0, (GLsizei)width, (GLsizei)height, GL_RGB, GL_UNSIGNED_BYTE, *img_pixels);
        for (uint32_t i = 0; i < height; i++) {
            for (uint32_t j = 0; j < width; j++) {
                const size_t cur = format_nchannels * ((height - i - 1) * width + j);
                std::fprintf(f, "%3d %3d %3d ", (*img_pixels)[cur], (*img_pixels)[cur + 1], (*img_pixels)[cur + 2]);
            }
            std::fprintf(f, "\n");
        }
        std::fclose(f);
    }

    void screenshot_png(const char* filename, uint32_t width, uint32_t height, GLubyte** img_pixels) noexcept {
        const size_t n = 3u * width * height;
        *img_pixels = (GLubyte*)std::realloc(*img_pixels, n);
        glReadPixels(0, 0, (GLsizei)width, (GLsizei)height, GL_RGB, GL_UNSIGNED_BYTE, *img_pixels);

        std::vector<GLubyte> flipped(n);
        for (uint32_t y = 0; y < height; ++y) {
            const uint32_t src_y = height - 1u - y;
            for (uint32_t x = 0; x < width; ++x) {
                const size_t dst = (y * width + x) * 3u;
                const size_t src = (src_y * width + x) * 3u;
                flipped[dst + 0] = (*img_pixels)[src + 0];
                flipped[dst + 1] = (*img_pixels)[src + 1];
                flipped[dst + 2] = (*img_pixels)[src + 2];
            }
        }
        write_rgb_png_file(filename, width, height, flipped.data());
    }

public:
    enum class TypesFormats { USE_PPM, USE_PNG };

    Screenshot() = default;

    ~Screenshot() {
        if (pixels != nullptr)
            std::free(pixels);
    }

    void print_screen(const std::string& filename, size_t width, size_t height, TypesFormats type) noexcept {
        if (width == 0 || height == 0)
            return;
        if (type == TypesFormats::USE_PNG || ends_with_ci(filename, ".png"))
            screenshot_png(filename.c_str(), (uint32_t)width, (uint32_t)height, &pixels);
        else
            screenshot_ppm(filename.c_str(), (uint32_t)width, (uint32_t)height, &pixels);
    }

    void print_screen(const std::string& filename, size_t width, size_t height) noexcept {
        TypesFormats fmt = ends_with_ci(filename, ".png") ? TypesFormats::USE_PNG : TypesFormats::USE_PPM;
        print_screen(filename, width, height, fmt);
    }
};

} // namespace utility
} // namespace easy_plot

#endif // EASY_PLOT_SCRENNSHOT_HPP_INCLUDED
