#include "sdlg.hpp"
#include "app.hpp"

int main(int, char **) {
    Sdl sdl;
    App app(&sdl);

    char image_path[1024] = "images/linuxmint_hawaii.jpg";
    bool failed_load = false;

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

        ImGui::Text("Image:");
        ImGui::SameLine();
        ImGui::InputText("##image_path", image_path, sizeof(image_path));
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            failed_load = !app.load_image(image_path);
        }
        if (failed_load) {
            ImGui::SameLine();
            ImGui::Text("(failed to load)");
        }

        ImGui::Text("Adjust:");
        if (ImGui::RadioButton("Adjust Width",
                               app.get_adjust() == Adjust_Width)) {
            app.set_adjust(Adjust_Width);
        }
        if (ImGui::RadioButton("Adjust Height",
                               app.get_adjust() == Adjust_Height)) {
            app.set_adjust(Adjust_Height);
        }
        if (ImGui::RadioButton("Adjust Fill",
                               app.get_adjust() == Adjust_Fill)) {
            app.set_adjust(Adjust_Fill);
        }

        if (ImGui::Button("Quit")) {
            running = false;
        }
        ImGui::End();
        sdl.clear();
        app.draw();
        sdl.draw();
    }
}
