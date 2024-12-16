#include "imgui/backends/imgui_impl_sdl2.h"
#include "imgui/backends/imgui_impl_sdlrenderer2.h"
#include "imgui/imgui.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

#if !SDL_VERSION_ATLEAST(2, 0, 17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

void imgui_new_frame() {
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void imgui_render() {
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
}

struct Sdl {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Event event;
    int width, height;
    int mousex, mousey;

    Sdl() {
        SDL_Init(SDL_INIT_VIDEO);
#ifdef SDL_HINT_IME_SHOW_UI
        SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif
        window =
            SDL_CreateWindow("main", 0, 0, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
        renderer = SDL_CreateRenderer(
            window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
        SDL_GetRendererOutputSize(renderer, &width, &height);
        texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET, width, height);
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        ImGui::StyleColorsDark();
        ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
        ImGui_ImplSDLRenderer2_Init(renderer);
    }

    ~Sdl() {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    void clear() {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderFillRect(renderer, NULL);
    }

    void draw() {
        imgui_render();
        SDL_RenderPresent(renderer);
    }

    bool poll_event() {
        for (;;) {
            if (!SDL_PollEvent(&event)) {
                return false;
            }
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_MOUSEBUTTONDOWN &&
                ImGui::GetIO().WantCaptureMouse) {
                continue;
            }
            if (event.type == SDL_KEYDOWN &&
                ImGui::GetIO().WantCaptureKeyboard) {
                continue;
            }
            if (event.type == SDL_MOUSEMOTION) {
                mousex = event.button.x;
                mousey = event.button.y;
            }
            return true;
        }
    }

    bool is_quit_event() {
        return event.type == SDL_QUIT ||
               (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window));
    }
};
