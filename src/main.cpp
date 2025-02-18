#include "imgui/imgui.h"
#include "processing.hpp"
#include "sdlg.hpp"
#include "app.hpp"

int main(int, char **) {
    Sdl sdl;
    App app(&sdl);

    char image_path[1024] = "images/linuxmint_ireland.jpg";
    bool failed_load = false;
    int kernel_dim[2] = {int(app.kernel_width), int(app.kernel_height)};

    bool running = true;
    while (running) {
        SDL_GetRendererOutputSize(sdl.renderer, &sdl.width, &sdl.height);
        app.readjust();

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
                case SDLK_w:
                case SDLK_UP:
                    app.add_pan(0, -1);
                    break;
                case SDLK_s:
                case SDLK_DOWN:
                    app.add_pan(0, 1);
                    break;
                case SDLK_a:
                case SDLK_LEFT:
                    app.add_pan(-1, 0);
                    break;
                case SDLK_d:
                case SDLK_RIGHT:
                    app.add_pan(1, 0);
                    break;
                case SDLK_q:
                    app.add_zoom(1.0);
                    break;
                case SDLK_e:
                    app.add_zoom(-1.0);
                    break;
                }
            case SDL_MOUSEWHEEL: {
                if (ImGui::GetIO().WantCaptureMouse) {
                    continue;
                }
                float amount = sdl.event.wheel.preciseY;
                app.add_zoom(amount);
                break;
            }
            }
        }
        imgui_begin();
        if (ImGui::Button("Quit")) {
            running = false;
        }

        ImGui::Text("Image:");
        ImGui::SameLine();
        bool wants_load_image = false;
        wants_load_image |=
            ImGui::InputText("##image_path", image_path, sizeof(image_path),
                             ImGuiInputTextFlags_EnterReturnsTrue);
        if (failed_load) {
            ImGui::SameLine();
            ImGui::Text("(failed to load)");
        }

        ImGui::Text("Threshold: ");
        ImGui::SameLine();
        ImGui::SliderInt("##threshold", &app.threshold, 0, 255);

        wants_load_image |= ImGui::Button("Load");
        ImGui::SameLine();
        if (ImGui::Button("Reset")) {
            app.reset_image();
        }

        if (wants_load_image) {
            failed_load = !app.load_image(image_path);
        }

        if (ImGui::CollapsingHeader("Zoom & Pan",
                                    ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            if (ImGui::Button("-")) {
                app.add_zoom(1.0);
            }
            ImGui::SameLine();
            if (ImGui::Button("^")) {
                app.add_pan(0.0, -1.0);
            }
            ImGui::SameLine();
            if (ImGui::Button("+")) {
                app.add_zoom(-1.0);
            }
            ImGui::SameLine();
            if (ImGui::Button("=")) {
                app.reset_zoom_pan();
            }

            if (ImGui::Button("<")) {
                app.add_pan(-1.0, 0.0);
            }
            ImGui::SameLine();
            if (ImGui::Button("v")) {
                app.add_pan(0.0, 1.0);
            }
            ImGui::SameLine();
            if (ImGui::Button(">")) {
                app.add_pan(1.0, 0.0);
            }
            ImGui::Unindent();
        }

        if (ImGui::CollapsingHeader("Kernel", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Indent();
            ImGui::BeginTable("##kernel", app.kernel_width);
            for (size_t row = 0; row < app.kernel_height; row++) {
                ImGui::TableNextRow();
                for (size_t col = 0; col < app.kernel_width; col++) {
                    ImGui::TableNextColumn();
                    size_t index = row * app.kernel_width + col;
                    ImGui::PushID(index);
                    ImGui::Checkbox("##chk", (bool *)&app.kernel[index]);
                    ImGui::PopID();
                }
            }
            ImGui::EndTable();

            ImGui::Text("Size  :");
            ImGui::SameLine();
            ImGui::PushItemWidth(64.0);
            if (ImGui::InputInt2("##kernel_width", kernel_dim)) {
                app.resize_kernel(kernel_dim[0], kernel_dim[1]);
            }
            ImGui::PopItemWidth();

            ImGui::PushItemWidth(64.0);
            ImGui::Text("Center:");
            ImGui::SameLine();
            ImGui::InputInt2("##kernel_center", app.kernel_center);
            ImGui::PopItemWidth();

            ImGui::PushItemWidth(128.0);
            if (ImGui::BeginCombo(
                    "##combo",
                    MORPHOLOGICAL_OPS_NAMES
                        [app.get_morphological_operation()])) // The second
                                                              // parameter is
                                                              // the label
                                                              // previewed
                                                              // before opening
                                                              // the combo.
            {
                if (ImGui::Selectable("EROSION",
                                      app.get_morphological_operation() ==
                                          EROSION)) {
                    app.change_morphological_operation(EROSION);
                }
                if (ImGui::Selectable("DILATION",
                                      app.get_morphological_operation() ==
                                          DILATION)) {
                    app.change_morphological_operation(DILATION);
                }
                if (ImGui::Selectable("OPENING",
                                      app.get_morphological_operation() ==
                                          OPENING)) {
                    app.change_morphological_operation(OPENING);
                }
                if (ImGui::Selectable("CLOSURE",
                                      app.get_morphological_operation() ==
                                          CLOSURE)) {
                    app.change_morphological_operation(CLOSURE);
                }

                ImGui::EndCombo();
            }
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ImGui::Button("Apply")) {
                app.apply_kernel();
            }
            ImGui::Unindent();
        }

        ImGui::End();
        sdl.clear();
        app.draw();
        sdl.draw();
    }
}
