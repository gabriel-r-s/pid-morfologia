#ifndef PROCESSING_HPP
#define PROCESSING_HPP

#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <array>
#include <cstdint>
#include <iostream>
#include <vector>

#define NOTHING 0xFFFFFFFF

enum Morphological_Operation {
    EROSION,
    DILATION,
    OPENING,
    CLOSURE
};

struct Image_Kernel {
    std::vector<uint8_t> pixels;
    size_t width, height;
    int center_x, center_y;
};

bool check_bounds(int x, int y, int width, int height) {
    return x > -1 && y > -1 && x < width && y < height;
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

void dilate_texture(SDL_Texture *texture)
{

}

void apply_opening_to_image(SDL_Texture *texture)
{

}

void apply_closing_to_image(SDL_Texture *texture)
{

}

#endif
