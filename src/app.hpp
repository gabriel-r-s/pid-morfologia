#include <vector>
#include "sdlg.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class App {
    Sdl *sdl;
    SDL_Texture *image, *processed;
    SDL_Rect image_rect;
    std::vector<bool> kernel;
    size_t kernel_width;

  public:
    App(Sdl *sdl)
        : sdl(sdl), image(nullptr), processed(nullptr),
          image_rect({0, 0, 0, 0}), kernel_width(3) {
        kernel = {0, 1, 0, 1, 1, 1, 0, 1, 0};
    }

    ~App() {
        SDL_DestroyTexture(image);
        SDL_DestroyTexture(processed);
    }

    void reset_image() {
        SDL_SetRenderTarget(sdl->renderer, processed);
        SDL_RenderCopy(sdl->renderer, image, nullptr, nullptr);
        SDL_SetRenderTarget(sdl->renderer, nullptr);
    }

    void adjust_image_rect(int w, int h) {
        float ratio = float(h) / sdl->height;

        float new_w = w / ratio;
        float new_h = h / ratio;
        float new_x = (sdl->width - new_w) / 2;
        float new_y = (sdl->height - new_h) / 2;

        image_rect = {int(new_x), int(new_y), int(new_w), int(new_h)};
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
        adjust_image_rect(w, h);
        reset_image();
        return true;
    }

    void draw() {
        if (processed == nullptr) {
            return;
        }
        SDL_RenderCopy(sdl->renderer, processed, nullptr, &image_rect);
    }
};