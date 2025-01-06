#ifndef PROCESSING_HPP
#define PROCESSING_HPP

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <cstdint>
#include <vector>

#define NOTHING 0xFFFFFFFF


enum Morphological_Operation {
    EROSION,
    DILATION,
    OPENING,
    CLOSURE
};

const char* MORPHOLOGICAL_OPS_NAMES[] = {"EROSION", "DILATION", "OPENING", "CLOSURE"};

struct Image_Kernel {
    std::vector<uint8_t> pixels;
    size_t width, height;
    int center_x, center_y;
};

bool check_bounds(int x, int y, int width, int height) {
    return x > -1 && y > -1 && x < width && y < height;
}

Uint32 get_square_mean(Uint32 *pixels, int x, int y, int width, int height, int pitch) {
    int pixels_amount = 0;
    SDL_Color color_mean = {0, 0, 0, 0};
    for (int i = y - 1; i < y + 1; i++) {
        for (int j = x - 1; j < x + 1; j++) {
            if (i > -1 && j > -1 && i < height && j < width) {
                pixels_amount++;
                color_mean.r += (pixels[i * pitch + j] >> 24) & 0xFF;
                color_mean.g += (pixels[i * pitch + j] >> 16) & 0xFF;
                color_mean.b += (pixels[i * pitch + j] >> 8) & 0xFF;
                color_mean.a += (pixels[i * pitch + j]) & 0xFF;
            }
        }
    }
    return ((Uint32)((float)color_mean.r / (float)pixels_amount) << 24)
        | ((Uint32)((float)color_mean.g / (float)pixels_amount) << 16)
        | ((Uint32)((float)color_mean.b / (float)pixels_amount) << 8)
        | 255;
}

void erode_texture(SDL_Texture *texture, int image_width, int image_height, Image_Kernel kernel)
{
    void *pixels;
    int pitch;
    if (SDL_LockTexture(texture, NULL, &pixels, &pitch) == 0) {
        Uint32 *pixel_array = (Uint32 *) pixels;
        std::vector<Uint32> pixel_array_copy;
        for (int i = 0; i < image_width * image_height; i++) {
            pixel_array_copy.push_back(pixel_array[i]);
        }
        int pitch_in_pixels = pitch / sizeof(Uint32);
        for (int y = 0; y < image_height; y++) {
            for (int x = 0; x < image_width; x++) {
                bool should_erode = false;
                for (size_t i = 0; i < kernel.height && !should_erode; i++) {
                    for (size_t j = 0; j < kernel.width && !should_erode; j++) {
                        int x_on_image = j - kernel.center_x + x;
                        int y_on_image = i - kernel.center_y + y;
                        if (!kernel.pixels[(int) i * (int) kernel.width + (int) j]) {
                            continue;
                        }
                        if (!check_bounds(x_on_image, y_on_image, image_width, image_height)) {
                            continue;
                        }
                        if (pixel_array[y_on_image * pitch_in_pixels + x_on_image] == NOTHING) {
                            should_erode = true;
                        }
                    }
                }
                if (should_erode) {
                    pixel_array_copy[y * pitch_in_pixels + x] = NOTHING;
                }
            }
        }
        for (int y = 0; y < image_height; y++) {
            for (int x = 0; x < image_width; x++) {
                pixel_array[y * pitch_in_pixels + x] = pixel_array_copy[y * pitch_in_pixels + x];
            }
        }
        SDL_UnlockTexture(texture);
    }
}

void dilate_texture(SDL_Texture *texture, int image_width, int image_height, Image_Kernel kernel)
{
    void *pixels;
    int pitch;
    std::vector<uint8_t> inversed_kernel_pixels(kernel.pixels.size(), 0);
    for (size_t i = 0; i < kernel.height; i++) {
        for (size_t j = 0; j < kernel.width; j++) {
            inversed_kernel_pixels[i * kernel.width + j] = kernel.pixels[(kernel.height - 1 - i) * kernel.width + (kernel.width - 1 - j)];
        }
    }
    if (SDL_LockTexture(texture, NULL, &pixels, &pitch) == 0) {
        Uint32 *pixel_array = (Uint32 *) pixels;
        std::vector<Uint32> pixel_array_copy;
        for (int i = 0; i < image_width * image_height; i++) {
            pixel_array_copy.push_back(pixel_array[i]);
        }
        int pitch_in_pixels = pitch / sizeof(Uint32);
        for (int y = 0; y < image_height; y++) {
            for (int x = 0; x < image_width; x++) {
                bool should_dilate = false;

                for (size_t i = 0; i < kernel.height && !should_dilate; i++) {
                    for (size_t j = 0; j < kernel.width && !should_dilate; j++) {
                        int x_on_image = j - kernel.center_x + x;
                        int y_on_image = i - kernel.center_y + y;
                        if (!inversed_kernel_pixels[(int) i * (int) kernel.width + (int) j]) {
                            continue;
                        }
                        if (!check_bounds(x_on_image, y_on_image, image_width, image_height)) {
                            continue;
                        }
                        if (pixel_array[y_on_image * pitch_in_pixels + x_on_image] != NOTHING) {
                            should_dilate = true;
                        }
                    }
                }
                if (should_dilate) {
                    pixel_array_copy[y * pitch_in_pixels + x] = get_square_mean(pixel_array, x, y, image_width, image_height, pitch_in_pixels);
                    // pixel_array_copy[y * pitch_in_pixels + x] = ((Uint32)color_mean.a << 24) | ((Uint32)color_mean.r << 16) | ((Uint32)color_mean.g << 8) | (Uint32)color_mean.b;
                }
            }
        }
        for (int y = 0; y < image_height; y++) {
            for (int x = 0; x < image_width; x++) {
                pixel_array[y * pitch_in_pixels + x] = pixel_array_copy[y * pitch_in_pixels + x];
            }
        }
        SDL_UnlockTexture(texture);
    }
}

void apply_opening_to_image(SDL_Texture *texture, int image_width, int image_height, Image_Kernel kernel)
{
    erode_texture(texture, image_width, image_height, kernel);
    dilate_texture(texture, image_width, image_height, kernel);
}

void apply_closing_to_image(SDL_Texture *texture, int image_width, int image_height, Image_Kernel kernel)
{
    dilate_texture(texture, image_width, image_height, kernel);
    erode_texture(texture, image_width, image_height, kernel);
}

#endif
