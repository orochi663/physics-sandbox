#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <glm/glm.hpp>
#include <memory>
#include <iostream>

#define NANOVG_GL3
#include <nanovg.h>
#include <nanovg_gl.h>

// Updated UI library includes with "ui/" prefix.
#include "ui/UIFactory.h"
#include "ui/UIManager.h"
#include "ui/SDLInputTranslator.h"
#include "ui/NanoVGRenderer.h"

int main(int, char**)
{
    // Initialize SDL (video and events)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Set OpenGL attributes for a 3.2 Core context (needed by NanoVG's GL3 backend)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // Create an SDL window with OpenGL support using SDL3
    SDL_Window* window = SDL_CreateWindow("UI Library Example", 1280, 720, SDL_WINDOW_OPENGL);
    if (!window) {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // Create an OpenGL context for the window
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        std::cerr << "Failed to create OpenGL context: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Initialize NanoVG with the GL3 backend
    NVGcontext* vg = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if (!vg) {
        std::cerr << "Could not initialize NanoVG." << std::endl;
        SDL_GL_DestroyContext(glContext);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Create our renderer instance using NanoVG
    ui::NanoVGRenderer renderer(vg);

    // Get the UIManager instance and set the SDL input translator
    ui::UIManager& uiManager = ui::UIManager::getInstance();
    uiManager.setInputTranslator(std::make_unique<ui::SDLInputTranslator>());

    // Create a UI canvas via UIFactory (instead of directly using std::make_unique)
    auto canvas = ui::UIFactory::createCanvas("canvas", 0);
    canvas->setPosition(glm::vec2(0.0f, 0.0f));
    canvas->setSize(glm::vec2(1280.0f, 720.0f));

    // Create a button with the label "Click Me" using its static create method
    auto button = ui::UIButton::create("Click Me");
    // Position the button near the center of the canvas
    button->setPosition(glm::vec2(540.0f, 335.0f));
    button->setSize(glm::vec2(200.0f, 50.0f));
    // Set a callback for when the button is clicked
    button->setOnClick([]() {
        SDL_Log("Button clicked!");
        });
    // Add the button to the canvas
    canvas->addChild(std::move(button));

    // Register the canvas with the UIManager so it gets updated and rendered
    uiManager.addCanvas(std::move(canvas));

    bool running = true;
    SDL_Event event;
    while (running)
    {
        // Process SDL events
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
            // Pass events to our UIManager (which uses SDLInputTranslator internally)
            uiManager.processInput(&event);
        }

        // Update UI logic
        uiManager.update();

        // Clear the screen (using OpenGL)
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render the UI
        uiManager.render(&renderer);

        // Swap buffers to display the rendered frame
        SDL_GL_SwapWindow(window);

        // Cap the frame rate roughly at 60 FPS
        SDL_Delay(16);
    }

    // Cleanup resources
    nvgDeleteGL3(vg);
    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
