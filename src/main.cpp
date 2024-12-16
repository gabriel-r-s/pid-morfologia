#include "sdlg.hpp"

int main(int, char **) {
    Sdl sdl;

    bool running = true;
    while (running) {
        while (sdl.poll_event()) {
            if (sdl.is_quit_event()) {
                running = false;
            }
            switch (sdl.event.type) {
            case SDL_MOUSEBUTTONDOWN:
                break;
            case SDL_KEYDOWN:
                switch (sdl.event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                }
            }
        }
        imgui_new_frame();
        ImGui::Begin("Menu");
        if (ImGui::Button("Quit")) {
            running = false;
        }
        ImGui::End();
        sdl.clear();
        sdl.draw();
    }
}
