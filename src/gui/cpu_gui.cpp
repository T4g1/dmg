#include "cpu_gui.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl3w.h>

#include "../dmg.h"
#include "../log.h"
#include "../cpu.h"


CPUGui::CPUGui(CPU *cpu) : cpu(cpu)
{
    running = false;

    sdl_window = nullptr;
}


bool CPUGui::init()
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
        "DMG - CPU GUI",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        CPUGUI_WIDTH,
        CPUGUI_HEIGHT,
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


void CPUGui::update()
{
    if (!running) {
        return;
    }

    // Display
    Uint32 current_ticks = SDL_GetTicks();
    if (current_ticks < last_refresh + 50) {
        return;
    }

    refresh_window();

    last_refresh = current_ticks;
}


void CPUGui::refresh_window()
{
    if (!running) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(sdl_window);
    ImGui::NewFrame();

    bool show_demo_window = true;
    ImGui::ShowDemoWindow(&show_demo_window);

    // Rendering
    ImGui::Render();
    SDL_GL_MakeCurrent(sdl_window, gl_context);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(sdl_window);
}


void CPUGui::handle_event(SDL_Event *event)
{
    if (!running) {
        return;
    }

    ImGui_ImplSDL2_ProcessEvent(event);
}


/**
 * @brief      Cleanup
 */
void CPUGui::close()
{
    if (!running) {
        return;
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(sdl_window);

    sdl_window = nullptr;
    running = false;
}


Uint32 CPUGui::get_window_id()
{
    return SDL_GetWindowID(sdl_window);
}
