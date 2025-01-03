#include <algorithm>
#include <vector>
#include "sdlg.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdint>

class App {
    Sdl *sdl;
    SDL_Texture *image, *processed;
    int image_w, image_h;
    SDL_Rect dest_rect;
    SDL_Rect crop_rect;
    float zoom;

  public:
    std::vector<uint8_t> kernel;
    size_t kernel_width, kernel_height;

    App(Sdl *sdl)
        : sdl(sdl), image(nullptr), processed(nullptr), image_w(0), image_h(0),
          dest_rect({0, 0, 0, 0}), zoom(1.0) {
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

    void add_zoom(float z) {
        zoom -= z * 0.1;
        zoom = std::max(zoom, 1.0f);
        recrop();
    }

    void add_pan(float x, float y) {
        crop_rect.x += x * image_w / 10 / zoom;
        crop_rect.y += y * image_h / 10 / zoom;
        recrop();
    }

    void reset_zoom_pan() {
        zoom = 1.0;
        crop_rect.x = 0;
        crop_rect.y = 0;
        recrop();
    }

    void recrop() {
        crop_rect.w = image_w / zoom;
        crop_rect.h = image_h / zoom;
        crop_rect.x = std::clamp(crop_rect.x, 0, image_w - crop_rect.w);
        crop_rect.y = std::clamp(crop_rect.y, 0, image_h - crop_rect.h);
    }

    void readjust() {
        float new_w_ah = float(image_w) * sdl->height / image_h;
        float new_h_ah = sdl->height;

        float new_w_aw = sdl->width;
        float new_h_aw = float(image_h) * sdl->width / image_w;

        float new_w, new_h;
        if (new_w_ah > sdl->width) {
            new_w = new_w_aw;
            new_h = new_h_aw;
        } else {
            new_w = new_w_ah;
            new_h = new_h_ah;
        }

        float new_x = (sdl->width - new_w) / 2;
        float new_y = (sdl->height - new_h) / 2;

        dest_rect = {int(new_x), int(new_y), int(new_w), int(new_h)};
    }

    void reset_image() {
        SDL_SetRenderTarget(sdl->renderer, processed);
        SDL_RenderCopy(sdl->renderer, image, nullptr, nullptr);
        SDL_SetRenderTarget(sdl->renderer, nullptr);
        crop_rect = {0, 0, image_w, image_h};
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
        readjust();
        reset_image();
        return true;
    }

    void draw() {
        if (processed == nullptr) {
            return;
        }
        SDL_RenderCopy(sdl->renderer, processed, &crop_rect, &dest_rect);
    }
};
