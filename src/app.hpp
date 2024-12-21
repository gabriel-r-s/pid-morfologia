#include <vector>
#include "sdlg.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdint>

enum Adjust {
    Adjust_Height,
    Adjust_Width,
    Adjust_Fill,
    Adjust_Fit,
};

class App {
    Sdl *sdl;
    SDL_Texture *image, *processed;
    SDL_Rect image_rect;
    int image_w, image_h;
    Adjust adjust;

  public:
    std::vector<uint8_t> kernel;
    size_t kernel_width, kernel_height;

    App(Sdl *sdl)
        : sdl(sdl), image(nullptr), processed(nullptr),
          image_rect({0, 0, 0, 0}), adjust(Adjust_Height) {
        kernel = {0, 1, 0, 1, 1, 1, 0, 1, 0};
        kernel_width = 3;
        kernel_height = 3;
    }

    ~App() {
        SDL_DestroyTexture(image);
        SDL_DestroyTexture(processed);
    }

    void resize_kernel(size_t w, size_t h) {
        if (w == 0 || h == 0) {
            return;
        }
        kernel_width = w;
        kernel_height = h;
        kernel.clear();
        kernel.resize(w * h);
    }

    void apply_kernel() {}

    Adjust get_adjust() { return adjust; }

    void set_adjust(Adjust val) {
        adjust = val;
        readjust();
    }

    void readjust() {
        float new_w, new_h;

        switch (adjust) {
        case Adjust_Height:
            new_w = float(image_w) * sdl->height / image_h;
            new_h = sdl->height;
            break;
        case Adjust_Width:
            new_w = sdl->width;
            new_h = float(image_h) * sdl->width / image_w;
            break;
        case Adjust_Fit: {
            float new_w_ah = float(image_w) * sdl->height / image_h;
            float new_h_ah = sdl->height;

            float new_w_aw = sdl->width;
            float new_h_aw = float(image_h) * sdl->width / image_w;

            if (new_w_ah > sdl->width) {
                new_w = new_w_aw;
                new_h = new_h_aw;
            } else {
                new_w = new_w_ah;
                new_h = new_h_ah;
            }

            break;
        }
        case Adjust_Fill:
        default:
            new_w = sdl->width;
            new_h = sdl->height;
            break;
        }

        float new_x = (sdl->width - new_w) / 2;
        float new_y = (sdl->height - new_h) / 2;

        image_rect = {int(new_x), int(new_y), int(new_w), int(new_h)};
    }

    void reset_image() {
        SDL_SetRenderTarget(sdl->renderer, processed);
        SDL_RenderCopy(sdl->renderer, image, nullptr, nullptr);
        SDL_SetRenderTarget(sdl->renderer, nullptr);
    }

    bool load_image(const char *path) {
        SDL_Texture *load_image = IMG_LoadTexture(sdl->renderer, path);
        if (load_image == nullptr) {
            return false;
        }
        image = load_image;
        if (processed != nullptr) {
            SDL_DestroyTexture(processed);
        }
        int w, h;
        SDL_QueryTexture(image, nullptr, nullptr, &w, &h);
        processed = SDL_CreateTexture(sdl->renderer, SDL_PIXELFORMAT_RGBA8888,
                                      SDL_TEXTUREACCESS_TARGET, w, h);
        image_w = w;
        image_h = h;
        set_adjust(adjust);
        reset_image();
        return true;
    }

    void draw() {
        if (processed == nullptr) {
            return;
        }
        SDL_Rect *adjust_rect = (adjust == Adjust_Fill) ? nullptr : &image_rect;
        SDL_RenderCopy(sdl->renderer, processed, nullptr, adjust_rect);
    }
};
