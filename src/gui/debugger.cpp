#include "debugger.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_memory_editor.h"
#include <GL/gl3w.h>
#include <iostream>
#include <fstream>

#include "../utils.h"
#include "../dmg.h"
#include "../log.h"
#include "../cpu.h"
#include "../mmu.h"
#include "../ppu.h"

static MemoryEditor memoryViewer;


Debugger::Debugger() : cpu(nullptr), mmu(nullptr), dmg(nullptr), ppu(nullptr)
{
    running = false;
    suspend_dmg = false;
    step_dmg = false;

    sdl_window = nullptr;

    for (size_t tile_id=0; tile_id<TOTAL_TILE_COUNT; tile_id++) {
        vram_tilemap[tile_id] = 0;
    }

    execution_speed = DEFAULT_SPEED;

    show_execution = false;
    show_memory = false;
    show_vram = false;
    show_ppu = false;
    show_cpu = false;
    show_breakpoints = false;

    breakpoint_activated = false;
    breakpoints.clear();
}


bool Debugger::init()
{
    if (cpu == nullptr) {
        error("No CPU linked with Debugger\n");
        return false;
    }

    if (mmu == nullptr) {
        error("No MMU linked with Debugger\n");
        return false;
    }

    if (dmg == nullptr) {
        error("No DMG linked with Debugger\n");
        return false;
    }

    if (ppu == nullptr) {
        error("No PPU linked with Debugger\n");
        return false;
    }

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
    io.Fonts->AddFontDefault();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameBorderSize = 1.0f;

    // Setup Platform/Renderer bindings
    ImGui_ImplSDL2_InitForOpenGL(sdl_window, gl_context);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);

    running = true;
    last_refresh = 0;

    load();

    return true;
}


/**
 * @brief      Update the debugger
 * @return     true if execution shall stops
 */
bool Debugger::update()
{
    dmg->set_speed(execution_speed);

    return suspend_dmg && !step_dmg;
}


void Debugger::draw()
{
    // Display
    Uint32 current_ticks = SDL_GetTicks();
    if (current_ticks < last_refresh + (1000 / FPS)) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(sdl_window);
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar())
    {
        ToggleButton("CPU", &show_cpu);
        ToggleButton("Memory", &show_memory);
        ToggleButton("Execution", &show_execution);
        ToggleButton("Breakpoints", &show_breakpoints);
        ToggleButton("PPU", &show_ppu);
        ToggleButton("VRAM", &show_vram);
        ToggleButton("Demo", &show_demo); // DEBUG
    }
    ImGui::EndMainMenuBar();

    if (show_cpu) display_registers();
    if (show_memory) display_memory();
    if (show_execution) display_execution();
    if (show_breakpoints) display_breakpoints();
    if (show_ppu) display_PPU_status();
    if (show_vram) display_VRAM_status();
    if (show_demo) ImGui::ShowDemoWindow(&show_demo); // DEBUG

    display_load_game();

    // Rendering
    ImGui::Render();
    SDL_GL_MakeCurrent(sdl_window, gl_context);
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(sdl_window);

    last_refresh = current_ticks;
}


void Debugger::handle_event(SDL_Event *event)
{
    ImGui_ImplSDL2_ProcessEvent(event);

    switch(event->type) {
    case SDL_KEYDOWN:
        switch(event->key.keysym.sym){
        case SDLK_F1:   // Show debugger
            show();
            break;
        case SDLK_F2:   // Activate breakpoints
            breakpoint_activated = !breakpoint_activated;
            break;
        case SDLK_F9:   // Resume/Break process
            suspend_dmg = !suspend_dmg;
            break;
        case SDLK_F10:  // Load another rom
            fileDialog.Open();
            break;
        }
        break;
    }
}


void Debugger::show()
{
    SDL_ShowWindow(sdl_window);
}


void Debugger::hide()
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

    if (gl_context != nullptr) {
        SDL_GL_DeleteContext(gl_context);
    }

    SDL_DestroyWindow(sdl_window);

    sdl_window = nullptr;
    running = false;

    save();
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

    ImGui::Begin(title, &show_memory);

    memoryViewer.DrawContents(mmu, mmu->ram, 0x10000);

    ImGui::End();
}


/**
 * @brief      Displays register values
 */
void Debugger::display_registers()
{
    if (!show_cpu) {
        return;
    }

    const char *title = "Registers";

    if (ImGui::Begin(title, &show_cpu)) {
        ImGui::BeginChild("registers");

        ImGui::Text("PC: 0x%04X", cpu->PC);
        ImGui::Separator();
        ImGui::Text("A: 0x%02X F: 0x%02X", cpu->reg[A], cpu->reg[F]);
        ImGui::Text("B: 0x%02X C: 0x%02X", cpu->reg[B], cpu->reg[C]);
        ImGui::Text("D: 0x%02X E: 0x%02X", cpu->reg[D], cpu->reg[E]);
        ImGui::Text("H: 0x%02X L: 0x%02X", cpu->reg[H], cpu->reg[L]);
        ImGui::Separator();
        ImGui::Text("AF: 0x%04X", cpu->reg16(AF));
        ImGui::Text("BC: 0x%04X", cpu->reg16(BC));
        ImGui::Text("DE: 0x%04X", cpu->reg16(DE));
        ImGui::Text("HL: 0x%04X", cpu->reg16(HL));
        ImGui::Text("SP: 0x%04X", cpu->reg16(SP));
        ImGui::Separator();

        ImGui::Columns(2, "boolean", false);

        ImGui::Text("IME:");
        ImGui::NextColumn();
        ColorBoolean(cpu->IME);
        ImGui::NextColumn();

        ImGui::Text("Halted:");
        ImGui::NextColumn();
        ColorBoolean(cpu->halted);
        ImGui::NextColumn();

        ImGui::Columns(1, "boolean", false);

        ImGui::EndChild();
    }

    ImGui::End();
}


/**
 * @brief      Displays execution and commented code
 */
void Debugger::display_execution()
{
    const char *title = "Execution";

    const size_t buffer_size = 75;
    char buffer[buffer_size];

    if (ImGui::Begin(title, &show_execution)) {
        ImGui::BeginChild("execution");

        ImGui::Columns(1, "control1", false);

        ImGui::Text("Clock: %zu", dmg->system_clock);
        ImGui::Text("Suspended:");
        ImGui::SameLine();
        ColorBoolean(suspend_dmg);

        ImGui::SetNextItemWidth(100);
        ImGui::DragInt("Speed", &execution_speed, 0.25f, 1, 5000, "%d");

        ImGui::Columns(3, "control2", false);

        if (ImGui::Button("Reset")) {
            dmg->reset();

            step_dmg = false;
        }

        ImGui::NextColumn();

        if (suspend_dmg) {
            if (ImGui::Button("Resume execution")) {
                suspend_dmg = false;
            }
        } else {
            if (ImGui::Button("Suspend execution")) {
                suspend_dmg = true;
            }
        }

        ImGui::NextColumn();

        if (ImGui::Button("Step execution")) {
            suspend_dmg = true;
            step_dmg = true;
        }

        ImGui::Columns(1, "code", false);

        ImGui::Separator();

        ImGui::Columns(3, "code", false);

        size_t operations = 0;
        uint32_t address = cpu->PC;
        char indicator = '>';
        while (operations < 0xFF) {
            operations += 1;

            // Out of bound
            if (address > 0xFFFF) {
                ImGui::Text(" ");
                ImGui::NextColumn();
                ImGui::Text(" ");
                ImGui::NextColumn();
                ImGui::Text(" ");
                ImGui::NextColumn();

                continue;
            }

            size_t increment = translate(buffer, buffer_size, address);

            ImGui::Text("%c %04X", indicator, address);

            ImGui::NextColumn();

            if (increment == 2) {
                ImGui::Text(
                    "%02X %02X",
                    *(mmu->ram + address),
                    *(mmu->ram + address + 1)
                );
            } else if (increment == 3) {
                ImGui::Text(
                    "%02X %02X %02X",
                    *(mmu->ram + address),
                    *(mmu->ram + address + 1),
                    *(mmu->ram + address + 2)
                );
            } else {
                ImGui::Text(
                    "%02X",
                    *(mmu->ram + address)
                );
            }

            ImGui::NextColumn();

            ImGui::Text("%s", buffer);

            ImGui::NextColumn();

            indicator = ' ';
            address += increment;
        }

        ImGui::Columns(1);

        ImGui::EndChild();
    }

    ImGui::End();
}


/**
 * @brief      Displays PPU status
 */
void Debugger::display_PPU_status()
{
    const char *title = "PPU Status";

    if (ImGui::Begin(title, &show_ppu)) {
        ImGui::BeginChild("status");

        ImGui::Columns(2, "boolean", false);

        ImGui::Text("LY:");
        ImGui::NextColumn();
        ImGui::Text("0x%02X", ppu->get_current_ly());
        ImGui::NextColumn();

        ImGui::Text("Mode:");
        ImGui::NextColumn();
        ImGui::Text("%s", ppu->get_current_mode());
        ImGui::NextColumn();

        ImGui::Columns(1, "boolean", false);

        ImGui::Separator();

        ImGui::Columns(2, "boolean", false);

        ImGui::Text("BG/Window tileset:");
        ImGui::NextColumn();
        ImGui::Text("0x%04X", ppu->bg_window_tile_data_address);
        ImGui::NextColumn();

        ImGui::Text("BG map data:");
        ImGui::NextColumn();
        ImGui::Text("0x%04X", ppu->bg_map_address);
        ImGui::NextColumn();

        ImGui::Text("Window map data:");
        ImGui::NextColumn();
        ImGui::Text("0x%04X", ppu->window_map_address);
        ImGui::NextColumn();

        ImGui::Columns(1, "boolean", false);

        ImGui::Separator();

        ImGui::Columns(2, "boolean", false);

        ImGui::Text("LCD Enabled:");
        ImGui::NextColumn();
        ColorBoolean(ppu->lcd_enabled);
        ImGui::NextColumn();

        ImGui::Text("BG Enabled:");
        ImGui::NextColumn();
        ColorBoolean(ppu->background_enabled);
        ImGui::NextColumn();

        ImGui::Text("Window Enabled:");
        ImGui::NextColumn();
        ColorBoolean(ppu->window_enabled);
        ImGui::NextColumn();

        ImGui::Text("Sprites Enabled:");
        ImGui::NextColumn();
        ColorBoolean(ppu->sprites_enabled);
        ImGui::NextColumn();

        ImGui::Columns(1, "boolean", false);

        ImGui::EndChild();
    }

    ImGui::End();
}


/**
 * @brief      Displays VRAM status (tiles, map data, ...)
 */
void Debugger::display_VRAM_status()
{
    const char *title = "VRAM Status";
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize;
    ImGui::SetNextWindowSizeConstraints(
        ImVec2(
            (DISPLAY_TILE_WIDTH + 1) * TILE_PER_COLUMN,
            (DISPLAY_TILE_HEIGHT + 1) * TILE_PER_ROW * TILESETS
        ),
        ImVec2(FLT_MAX, FLT_MAX)
    );
    if (ImGui::Begin(title, &show_vram, flags)) {
        ImGui::BeginChild("status");

        if (ImGui::BeginTabBar("tabs_bar", ImGuiTabBarFlags_None))
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1,1));
            if (ImGui::BeginTabItem("Tiles"))
            {
                ImGui::BeginChild("tiles_container");
                ImGui::Columns(TILE_PER_COLUMN, "boolean", false);

                glDeleteTextures(TOTAL_TILE_COUNT, vram_tilemap);

                for (size_t tile_id=0; tile_id<TOTAL_TILE_COUNT; tile_id++) {
                    uint8_t pixels[RGB_TILE_SIZE];
                    ppu->draw_tile(pixels, tile_id);
                    vram_tilemap[tile_id] = create_texture(pixels, TILE_WIDTH, TILE_HEIGHT);

                    ImageTimeHoverable((ImTextureID)(intptr_t)vram_tilemap[tile_id]);
                    ImGui::NextColumn();
                }

                ImGui::Columns(1, "boolean", false);

                ImGui::EndChild();
                ImGui::EndTabItem();
            }

            ImGui::PopStyleVar();

            ImGui::EndTabBar();
        }

        ImGui::EndChild();
    }

    ImGui::End();
}


/**
 * @brief      Display breakpoint settings
 */
void Debugger::display_breakpoints()
{
    const char *title = "Breakpoints";

    const ImU16 step1 = 0x0001;
    const ImU16 step10 = 0x0010;
    static ImU16 breakpoint_address = 0x0000;

    if (ImGui::Begin(title, &show_breakpoints)) {
        ToggleButton("Toggle breakpoints", &breakpoint_activated);
        ImGui::SetNextItemWidth(100);
        ImGui::InputScalar("Address", ImGuiDataType_U16, &breakpoint_address, &step1, &step10, "%04X", ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
        ImGui::SameLine();
        if (ImGui::Button("Add")) {
            bool unique = true;
            // Not already in the list
            for (auto breakpoint : breakpoints) {
                if (breakpoint.address == breakpoint_address) {
                    unique = false;
                }
            }

            if (unique) {
                Breakpoint breakpoint = {
                    .address = (uint16_t)breakpoint_address,
                    .read = false,
                    .write = false
                };

                breakpoints.push_back(breakpoint);
            }
        }
        ImGui::Separator();
        ImGui::Columns(4, "breakpoints", false);

        for (size_t i=0; i<breakpoints.size(); i++) {
            Breakpoint *breakpoint = &breakpoints[i];
            ImGui::PushID(breakpoint);

            ImGui::Text("0x%04X", breakpoint->address); ImGui::NextColumn();
            ToggleButton("read", &breakpoint->read); ImGui::NextColumn();
            ToggleButton("write", &breakpoint->write); ImGui::NextColumn();
            if (ImGui::Button("Delete")) {
                breakpoints.erase(breakpoints.begin() + i);
            }
            ImGui::NextColumn();
            ImGui::PopID();
        }

        ImGui::Columns(1);
    }

    ImGui::End();
}


/**
 * @brief      Displays a file dialog to load a rom
 */
void Debugger::display_load_game()
{
    fileDialog.Display();

    if(fileDialog.HasSelected()) {
        dmg->load_rom(fileDialog.GetSelected().string());
        fileDialog.ClearSelected();
    }
}


/**
 * @brief      Translate current opcode into text
 * @param      buffer   The buffer
 * @param[in]  size     The size of buffer
 * @param[in]  address  The address of current opcode
 * @return     Next opcode address
 */
uint16_t Debugger::translate(char buffer[], size_t size, uint16_t address)
{
    uint8_t d0 = *(mmu->ram + address + 1);
    uint8_t d1 = *(mmu->ram + address + 2);

    switch(*(mmu->ram + address)) {
    case 0x00: TRANSLATION(1, "NOP");
    case 0x01: TRANSLATION(3, "LD BC,0x%02X%02X", d1, d0);
    case 0x02: TRANSLATION(1, "LD (BC),A");
    case 0x03: TRANSLATION(1, "INC BC");
    case 0x04: TRANSLATION(1, "INC B");
    case 0x05: TRANSLATION(1, "DEC B");
    case 0x06: TRANSLATION(2, "LD B,0x%02X", d0);
    case 0x07: TRANSLATION(1, "RLCA");
    case 0x08: TRANSLATION(3, "LD (0x%02X%02X),SP", d1, d0);
    case 0x09: TRANSLATION(1, "ADD HL,BC");
    case 0x0A: TRANSLATION(1, "LD A,(BC)");
    case 0x0B: TRANSLATION(1, "DEC BC");
    case 0x0C: TRANSLATION(1, "INC C");
    case 0x0D: TRANSLATION(1, "DEC C");
    case 0x0E: TRANSLATION(2, "LD C,0x%02X", d0);
    case 0x0F: TRANSLATION(1, "RRCA");
    case 0x10: TRANSLATION(1, "00 STOP");
    case 0x11: TRANSLATION(3, "LD DE,0x%02X%02X", d1, d0);
    case 0x12: TRANSLATION(1, "LD (DE),A");
    case 0x13: TRANSLATION(1, "INC DE");
    case 0x14: TRANSLATION(1, "INC D");
    case 0x15: TRANSLATION(1, "DEC D");
    case 0x16: TRANSLATION(2, "LD D,0x%02X", d0);
    case 0x17: TRANSLATION(1, "RLA");
    case 0x18: TRANSLATION(2, "JR 0x%02X", d0);
    case 0x19: TRANSLATION(1, "ADD HL,DE");
    case 0x1A: TRANSLATION(1, "LD A,(DE)");
    case 0x1B: TRANSLATION(1, "DEC DE");
    case 0x1C: TRANSLATION(1, "INC E");
    case 0x1D: TRANSLATION(1, "DEC E");
    case 0x1E: TRANSLATION(2, "LD E,0x%02X", d0);
    case 0x1F: TRANSLATION(1, "RRA");
    case 0x20: TRANSLATION(2, "JR NZ,0x%02X", d0);
    case 0x21: TRANSLATION(3, "LD HL,0x%02X%02X", d1, d0);
    case 0x22: TRANSLATION(1, "LD (HLI),A");
    case 0x23: TRANSLATION(1, "INC HL");
    case 0x24: TRANSLATION(1, "INC H");
    case 0x25: TRANSLATION(1, "DEC H");
    case 0x26: TRANSLATION(2, "LD H,0x%02X", d0);
    case 0x27: TRANSLATION(1, "DAA");
    case 0x28: TRANSLATION(2, "JR Z,0x%02X", d0);
    case 0x29: TRANSLATION(1, "ADD HL,HL");
    case 0x2A: TRANSLATION(1, "LD A,(HLI)");
    case 0x2B: TRANSLATION(1, "DEC HL");
    case 0x2C: TRANSLATION(1, "INC L");
    case 0x2D: TRANSLATION(1, "DEC L");
    case 0x2E: TRANSLATION(2, "LD L,0x%02X", d0);
    case 0x2F: TRANSLATION(1, "CPL");
    case 0x30: TRANSLATION(2, "JR NC,0x%02X", d0);
    case 0x31: TRANSLATION(3, "LD SP,0x%02X%02X", d1, d0);
    case 0x32: TRANSLATION(1, "LD (HLD),A");
    case 0x33: TRANSLATION(1, "INC SP");
    case 0x34: TRANSLATION(1, "INC (HL)");
    case 0x35: TRANSLATION(1, "DEC (HL)");
    case 0x36: TRANSLATION(2, "LD (HL),0x%02X", d0);
    case 0x37: TRANSLATION(1, "SCF");
    case 0x38: TRANSLATION(2, "JR C,0x%02X", d0);
    case 0x39: TRANSLATION(1, "ADD HL,SP");
    case 0x3A: TRANSLATION(1, "LD A,(HLD)");
    case 0x3B: TRANSLATION(1, "DEC SP");
    case 0x3C: TRANSLATION(1, "INC A");
    case 0x3D: TRANSLATION(1, "DEC A");
    case 0x3E: TRANSLATION(2, "LD A,0x%02X", d0);
    case 0x3F: TRANSLATION(1, "CCF");
    case 0x40: TRANSLATION(1, "LD B,B");
    case 0x41: TRANSLATION(1, "LD B,C");
    case 0x42: TRANSLATION(1, "LD B,D");
    case 0x43: TRANSLATION(1, "LD B,E");
    case 0x44: TRANSLATION(1, "LD B,H");
    case 0x45: TRANSLATION(1, "LD B,L");
    case 0x46: TRANSLATION(1, "LD B,(HL)");
    case 0x47: TRANSLATION(1, "LD B,A");
    case 0x48: TRANSLATION(1, "LD C,B");
    case 0x49: TRANSLATION(1, "LD C,C");
    case 0x4A: TRANSLATION(1, "LD C,D");
    case 0x4B: TRANSLATION(1, "LD C,E");
    case 0x4C: TRANSLATION(1, "LD C,H");
    case 0x4D: TRANSLATION(1, "LD C,L");
    case 0x4E: TRANSLATION(1, "LD C,(HL)");
    case 0x4F: TRANSLATION(1, "LD C,A");
    case 0x50: TRANSLATION(1, "LD D,B");
    case 0x51: TRANSLATION(1, "LD D,C");
    case 0x52: TRANSLATION(1, "LD D,D");
    case 0x53: TRANSLATION(1, "LD D,E");
    case 0x54: TRANSLATION(1, "LD D,H");
    case 0x55: TRANSLATION(1, "LD D,L");
    case 0x56: TRANSLATION(1, "LD D,(HL)");
    case 0x57: TRANSLATION(1, "LD D,A");
    case 0x58: TRANSLATION(1, "LD E,B");
    case 0x59: TRANSLATION(1, "LD E,C");
    case 0x5A: TRANSLATION(1, "LD E,D");
    case 0x5B: TRANSLATION(1, "LD E,E");
    case 0x5C: TRANSLATION(1, "LD E,H");
    case 0x5D: TRANSLATION(1, "LD E,L");
    case 0x5E: TRANSLATION(1, "LD E,(HL)");
    case 0x5F: TRANSLATION(1, "LD E,A");
    case 0x60: TRANSLATION(1, "LD H,B");
    case 0x61: TRANSLATION(1, "LD H,C");
    case 0x62: TRANSLATION(1, "LD H,D");
    case 0x63: TRANSLATION(1, "LD H,E");
    case 0x64: TRANSLATION(1, "LD H,H");
    case 0x65: TRANSLATION(1, "LD H,L");
    case 0x66: TRANSLATION(1, "LD H,(HL)");
    case 0x67: TRANSLATION(1, "LD H,A");
    case 0x68: TRANSLATION(1, "LD L,B");
    case 0x69: TRANSLATION(1, "LD L,C");
    case 0x6A: TRANSLATION(1, "LD L,D");
    case 0x6B: TRANSLATION(1, "LD L,E");
    case 0x6C: TRANSLATION(1, "LD L,H");
    case 0x6D: TRANSLATION(1, "LD L,L");
    case 0x6E: TRANSLATION(1, "LD L,(HL)");
    case 0x6F: TRANSLATION(1, "LD H,A");
    case 0x70: TRANSLATION(1, "LD (HL),B");
    case 0x71: TRANSLATION(1, "LD (HL),C");
    case 0x72: TRANSLATION(1, "LD (HL),D");
    case 0x73: TRANSLATION(1, "LD (HL),E");
    case 0x74: TRANSLATION(1, "LD (HL),H");
    case 0x75: TRANSLATION(1, "LD (HL),L");
    case 0x76: TRANSLATION(1, "HALT");
    case 0x77: TRANSLATION(1, "LD (HL),A");
    case 0x78: TRANSLATION(1, "LD A,B");
    case 0x79: TRANSLATION(1, "LD A,C");
    case 0x7A: TRANSLATION(1, "LD A,D");
    case 0x7B: TRANSLATION(1, "LD A,E");
    case 0x7C: TRANSLATION(1, "LD A,H");
    case 0x7D: TRANSLATION(1, "LD A,L");
    case 0x7E: TRANSLATION(1, "LD A,(HL)");
    case 0x7F: TRANSLATION(1, "LD A,A");
    case 0x80: TRANSLATION(1, "ADD A,B");
    case 0x81: TRANSLATION(1, "ADD A,C");
    case 0x82: TRANSLATION(1, "ADD A,D");
    case 0x83: TRANSLATION(1, "ADD A,E");
    case 0x84: TRANSLATION(1, "ADD A,H");
    case 0x85: TRANSLATION(1, "ADD A,L");
    case 0x86: TRANSLATION(1, "ADD A,(HL)");
    case 0x87: TRANSLATION(1, "ADD A,A");
    case 0x88: TRANSLATION(1, "ADC A,B");
    case 0x89: TRANSLATION(1, "ADC A,C");
    case 0x8A: TRANSLATION(1, "ADC A,D");
    case 0x8B: TRANSLATION(1, "ADC A,E");
    case 0x8C: TRANSLATION(1, "ADC A,H");
    case 0x8D: TRANSLATION(1, "ADC A,L");
    case 0x8E: TRANSLATION(1, "ADC A,(HL)");
    case 0x8F: TRANSLATION(1, "ADC A,A");
    case 0x90: TRANSLATION(1, "SUB B");
    case 0x91: TRANSLATION(1, "SUB C");
    case 0x92: TRANSLATION(1, "SUB D");
    case 0x93: TRANSLATION(1, "SUB E");
    case 0x94: TRANSLATION(1, "SUB H");
    case 0x95: TRANSLATION(1, "SUB L");
    case 0x97: TRANSLATION(1, "SUB A");
    case 0x98: TRANSLATION(1, "SBC A,B");
    case 0x99: TRANSLATION(1, "SBC A,C");
    case 0x9A: TRANSLATION(1, "SBC A,D");
    case 0x9B: TRANSLATION(1, "SBC A,E");
    case 0x9C: TRANSLATION(1, "SBC A,H");
    case 0x9D: TRANSLATION(1, "SBC A,L");
    case 0x9E: TRANSLATION(1, "SBC A,(HL)");
    case 0x9F: TRANSLATION(1, "SBC A,A");
    case 0xA0: TRANSLATION(1, "AND B");
    case 0xA1: TRANSLATION(1, "AND C");
    case 0xA2: TRANSLATION(1, "AND D");
    case 0xA3: TRANSLATION(1, "AND E");
    case 0xA4: TRANSLATION(1, "AND H");
    case 0xA5: TRANSLATION(1, "AND L");
    case 0xA6: TRANSLATION(1, "AND (HL)");
    case 0xA7: TRANSLATION(1, "AND A");
    case 0xA8: TRANSLATION(1, "XOR B");
    case 0xA9: TRANSLATION(1, "XOR C");
    case 0xAA: TRANSLATION(1, "XOR D");
    case 0xAB: TRANSLATION(1, "XOR E");
    case 0xAC: TRANSLATION(1, "XOR H");
    case 0xAD: TRANSLATION(1, "XOR L");
    case 0xAE: TRANSLATION(1, "XOR (HL)");
    case 0xAF: TRANSLATION(1, "XOR A");
    case 0xB0: TRANSLATION(1, "OR B");
    case 0xB1: TRANSLATION(1, "OR C");
    case 0xB2: TRANSLATION(1, "OR D");
    case 0xB3: TRANSLATION(1, "OR E");
    case 0xB4: TRANSLATION(1, "OR H");
    case 0xB5: TRANSLATION(1, "OR L");
    case 0xB6: TRANSLATION(1, "OR (HL)");
    case 0xB7: TRANSLATION(1, "OR A");
    case 0xB8: TRANSLATION(1, "CP B");
    case 0xB9: TRANSLATION(1, "CP C");
    case 0xBA: TRANSLATION(1, "CP D");
    case 0xBB: TRANSLATION(1, "CP E");
    case 0xBC: TRANSLATION(1, "CP H");
    case 0xBD: TRANSLATION(1, "CP L");
    case 0xBE: TRANSLATION(1, "CP (HL)");
    case 0xBF: TRANSLATION(1, "CP A");
    case 0xC0: TRANSLATION(1, "RET NZ");
    case 0xC1: TRANSLATION(1, "POP BC");
    case 0xC2: TRANSLATION(3, "JP NZ,0x%02X%02X", d1, d0);
    case 0xC3: TRANSLATION(3, "JP 0x%02X%02X", d1, d0);
    case 0xC4: TRANSLATION(3, "CALL NZ,0x%02X%02X", d1, d0);
    case 0xC5: TRANSLATION(1, "PUSH BC");
    case 0xC6: TRANSLATION(2, "ADD A,0x%02X", d0);
    case 0xC7: TRANSLATION(1, "RST $00");
    case 0xC8: TRANSLATION(1, "RET Z");
    case 0xC9: TRANSLATION(1, "RET");
    case 0xCA: TRANSLATION(3, "JP Z,0x%02X%02X", d1, d0);
    case 0xCB: TRANSLATION(2, "CB %02X", d0);
    case 0xCC: TRANSLATION(3, "CALL Z,0x%02X%02X", d1, d0);
    case 0xCD: TRANSLATION(3, "CALL 0x%02X%02X", d1, d0);
    case 0xCE: TRANSLATION(2, "ADC A,0x%02X", d0);
    case 0xCF: TRANSLATION(1, "RST $08");
    case 0xD0: TRANSLATION(1, "RET NC");
    case 0xD1: TRANSLATION(1, "POP DE");
    case 0xD2: TRANSLATION(3, "JP NC,0x%02X%02X", d1, d0);
    case 0xD4: TRANSLATION(3, "CALL NC,0x%02X%02X", d1, d0);
    case 0xD5: TRANSLATION(1, "PUSH DE");
    case 0xD6: TRANSLATION(2, "SUB 0x%02X", d0);
    case 0xD7: TRANSLATION(1, "RST $10");
    case 0xD8: TRANSLATION(1, "RET C");
    case 0xD9: TRANSLATION(1, "RETI");
    case 0xDA: TRANSLATION(3, "JP C,0x%02X%02X", d1, d0);
    case 0xDC: TRANSLATION(3, "CALL C,0x%02X%02X", d1, d0);
    case 0xDE: TRANSLATION(2, "SBC A,0x%02X", d0);
    case 0xDF: TRANSLATION(1, "RST $18");
    case 0xE0: TRANSLATION(2, "LD (0xFF00+0x%02X),A", d0);
    case 0xE1: TRANSLATION(1, "POP HL");
    case 0xE2: TRANSLATION(1, "LD (0xFF00+C),A");
    case 0xE5: TRANSLATION(1, "PUSH HL");
    case 0xE6: TRANSLATION(2, "AND 0x%02X", d0);
    case 0xE7: TRANSLATION(1, "RST $20");
    case 0xE8: TRANSLATION(1, "ADD SP,xx");
    case 0xE9: TRANSLATION(1, "JP (HL)");
    case 0xEA: TRANSLATION(3, "LD (0x%02X%02X),A", d1, d0);
    case 0xEE: TRANSLATION(2, "XOR 0x%02X", d0);
    case 0xEF: TRANSLATION(1, "RST $28");
    case 0xF0: TRANSLATION(2, "LD A,(0xFF00+0x%02X)", d0);
    case 0xF1: TRANSLATION(1, "POP AF");
    case 0xF2: TRANSLATION(1, "LD A,(0xFF00+C)");
    case 0xF3: TRANSLATION(1, "DI");
    case 0xF5: TRANSLATION(1, "PUSH AF");
    case 0xF6: TRANSLATION(2, "OR 0x%02X", d0);
    case 0xF7: TRANSLATION(1, "RST $30");
    case 0xF8: TRANSLATION(1, "LD HL,SP");
    case 0xF9: TRANSLATION(1, "LD SP,HL");
    case 0xFA: TRANSLATION(3, "LD A,(0x%02X%02X)", d1, d0);
    case 0xFB: TRANSLATION(1, "EI");
    case 0xFE: TRANSLATION(2, "CP 0x%02X", d0);
    case 0xFF: TRANSLATION(1, "RST $38");
    }

    TRANSLATION(1, " ");
}


void Debugger::ColorBoolean(bool condition)
{
    ImVec4 active = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    ImVec4 inactive = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

    ImVec4 color = inactive;
    if (condition) {
        color = active;
    }

    ImGui::ColorButton("MyColor##3c", color,
        ImGuiColorEditFlags_NoTooltip |
        ImGuiColorEditFlags_NoDragDrop |
        ImGuiColorEditFlags_NoOptions
    );
}


void Debugger::ToggleButton(const char *text, bool *boolean)
{
    ImVec4 active = ImVec4(0.0f, 1.0f, 0.0f, 0.3f);
    ImVec4 inactive = ImVec4(1.0f, 0.0f, 0.0f, 0.3f);

    if (*boolean) {
        ImGui::PushStyleColor(ImGuiCol_Button, active);
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, inactive);
    }

    if (ImGui::Button(text)) {
        *boolean = !*boolean;
    }

    ImGui::PopStyleColor(1);
}


void Debugger::ImageTimeHoverable(ImTextureID texture)
{
    ImGui::Image(texture, ImVec2(DISPLAY_TILE_WIDTH, DISPLAY_TILE_HEIGHT));

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        float region_sz = 32.0f;
        float zoom = 2.0f;
        ImVec2 uv0 = ImVec2(0.0f, 0.f);
        ImVec2 uv1 = ImVec2(1.0f, 1.0f);
        ImGui::Image(
            texture,
            ImVec2(region_sz * zoom, region_sz * zoom),
            uv0,
            uv1,
            ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
            ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
        );
        ImGui::EndTooltip();
    }
}


size_t Debugger::get_speed()
{
    return execution_speed;
}


void Debugger::set_speed(size_t speed)
{
    execution_speed = speed;
}


/**
 * @brief      Called by MMU when a write occurs
 * @param[in]  address  The address at which we wrote
 */
void Debugger::feed_memory_write(uint16_t address)
{
    for (auto breakpoint : breakpoints) {
        if (breakpoint.write && breakpoint.address == address) {
            suspend_dmg = true;
            return;
        }
    }
}


/**
 * @brief      Called when a read occurs
 * @param[in]  address  The address at which we read
 */
void Debugger::feed_memory_read(uint16_t address)
{
    for (auto breakpoint : breakpoints) {
        if (breakpoint.read && breakpoint.address == address) {
            suspend_dmg = true;
            return;
        }
    }
}


/**
 * @brief      Save breakpoints for next session
 */
void Debugger::save()
{
    std::ofstream file;
    file.open(DEBUGGER_SAVE, std::ios::binary);

    for (auto breakpoint : breakpoints) {
        file.write(reinterpret_cast<const char*>(&breakpoint), sizeof(Breakpoint));
    }

    file.close();
}


/**
 * @brief      Loads breakpoints from previous session
 */
void Debugger::load()
{
    std::ifstream file;
    file.open(DEBUGGER_SAVE, std::ios::binary);

    while (!file.eof()) {
        int c = file.peek();
        if (c == EOF) {
            break;
        }

        Breakpoint breakpoint;
        file.read(reinterpret_cast<char*>(&breakpoint), sizeof(Breakpoint));
        breakpoints.push_back(breakpoint);
    }

    file.close();
}


void Debugger::set_cpu(CPU *cpu)
{
    this->cpu = cpu;
}


void Debugger::set_mmu(MMU *mmu)
{
    this->mmu = mmu;
}


void Debugger::set_dmg(DMG *dmg)
{
    this->dmg = dmg;
}


void Debugger::set_ppu(PPU *ppu)
{
    this->ppu = ppu;
}
