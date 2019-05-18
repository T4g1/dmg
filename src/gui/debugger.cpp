#include "debugger.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl3w.h>

#include "../dmg.h"
#include "../log.h"
#include "../cpu.h"
#include "../mmu.h"


Debugger::Debugger(CPU *cpu, MMU *mmu) : cpu(cpu), mmu(mmu)
{
    running = false;

    sdl_window = nullptr;
}


bool Debugger::init()
{
    // GL 3.0 + GLSL 130
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    sdl_window = SDL_CreateWindow(
        "DMG - Debugger",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DEBUGGER_WIDTH,
        DEBUGGER_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
    );
    if(sdl_window == NULL) {
        error("CPU GUI: nable to create a display window\n");
        return false;
    }

    gl_context = SDL_GL_CreateContext(sdl_window);
    if (!gl_context) {
        error("Unable to init GL/SDL context!\n");
        return false;
    }

    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Initialize OpenGL loader
    if (gl3wInit() != 0) {
        error("Failed to initialize OpenGL loader!\n");
        return false;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(sdl_window, gl_context);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    running = true;
    last_refresh = 0;

    return true;
}


void Debugger::update()
{
    // Display
    Uint32 current_ticks = SDL_GetTicks();
    if (current_ticks < last_refresh + 50) {
        return;
    }

    refresh_window();

    last_refresh = current_ticks;
}


void Debugger::refresh_window()
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(sdl_window);
    ImGui::NewFrame();

    display_memory();

    // Rendering
    ImGui::Render();
    SDL_GL_MakeCurrent(sdl_window, gl_context);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(sdl_window);
}


void Debugger::handle_event(SDL_Event *event)
{
    ImGui_ImplSDL2_ProcessEvent(event);
}


void Debugger::close()
{
    SDL_HideWindow(sdl_window);
}


/**
 * @brief      Cleanup
 */
void Debugger::quit()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(sdl_window);

    sdl_window = nullptr;
    running = false;
}


Uint32 Debugger::get_window_id()
{
    return SDL_GetWindowID(sdl_window);
}


/**
 * @brief      Shows memory explorer
 */
void Debugger::display_memory()
{
    const char *title = "Memory";

    if (ImGui::Begin(title)) {
        ImGui::BeginChild("mem");

        uint32_t address = 0x0000;
        do {
            ImGui::Text(
                "0x%04X: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
                address,
                mmu->get(address),
                mmu->get(address + 1),
                mmu->get(address + 2),
                mmu->get(address + 3),
                mmu->get(address + 4),
                mmu->get(address + 5),
                mmu->get(address + 6),
                mmu->get(address + 7)
            );

            address += 0x0008;
        } while (address < 0xFFFF);

        ImGui::EndChild();
    }

    ImGui::End();
}
